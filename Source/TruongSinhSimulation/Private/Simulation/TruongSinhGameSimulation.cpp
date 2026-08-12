#include "Simulation/TruongSinhGameSimulation.h"

#include "Containers/StringConv.h"
#include "Hash/Blake3.h"

namespace TruongSinhSimulationIds
{
constexpr TCHAR EventTimeAdvanced[] = TEXT("core.time_advanced");
constexpr TCHAR EventCultivationCommitted[] = TEXT("cultivation.committed");
constexpr TCHAR EventResolvedActivityCommitted[] = TEXT("activity.committed");
constexpr TCHAR ReasonDuplicateCommand[] = TEXT("core.reject.duplicate_command");
constexpr TCHAR ReasonInvalidCommand[] = TEXT("core.reject.invalid_command");
constexpr TCHAR ReasonInvalidPayload[] = TEXT("core.reject.invalid_payload");
constexpr TCHAR ReasonActivityPrecondition[] = TEXT("activity.reject.precondition");
constexpr TCHAR ReasonOverflow[] = TEXT("core.reject.overflow");
constexpr TCHAR ReasonRevisionMismatch[] = TEXT("core.reject.revision_mismatch");
constexpr TCHAR ReasonUnknownAction[] = TEXT("core.reject.unknown_action");
}

const TCHAR* FTruongSinhGameSimulation::AdvanceTimeActionId = TEXT("world.advance_time");
const TCHAR* FTruongSinhGameSimulation::CommitCultivationActionId = TEXT("cultivation.commit_resolved");
const TCHAR* FTruongSinhGameSimulation::CommitResolvedActivityActionId = TEXT("activity.commit_resolved");

FTruongSinhSimulationState FTruongSinhGameSimulation::CreateNewGame(const int64 MasterSeed)
{
    FTruongSinhSimulationState State;
    State.Rng.MasterSeed = MasterSeed;
    State.Rng.AlgorithmVersion = 1;
    State.WorldLayerId.Value = TEXT("world.lower_realm");
    State.Soul = FTruongSinhLifeRules::CreateInitialSoul();
    State.CurrentVessel = FTruongSinhLifeRules::CreateInitialVessel();
    return State;
}

FTruongSinhActionResult FTruongSinhGameSimulation::Execute(
    FTruongSinhSimulationState& InOutState,
    const FTruongSinhActionCommand& Command)
{
    if (!Command.CommandId.IsValid() || !Command.ActionId.IsValid() ||
        !Command.InstigatorId.IsValid() || Command.Sequence < 0)
    {
        return Reject(InOutState, Command, TruongSinhSimulationIds::ReasonInvalidCommand);
    }
    if (InOutState.CommittedCommandIds.Contains(Command.CommandId))
    {
        return Reject(InOutState, Command, TruongSinhSimulationIds::ReasonDuplicateCommand);
    }
    if (Command.ExpectedWorldRevision != InOutState.WorldRevision)
    {
        return Reject(InOutState, Command, TruongSinhSimulationIds::ReasonRevisionMismatch);
    }
    const FTruongSinhAdvanceTimePayload* AdvancePayload =
        Command.ActionId.Value == AdvanceTimeActionId ? Command.Payload.GetPtr<FTruongSinhAdvanceTimePayload>() : nullptr;
    const FTruongSinhCultivationCommitPayload* CultivationPayload =
        Command.ActionId.Value == CommitCultivationActionId ?
            Command.Payload.GetPtr<FTruongSinhCultivationCommitPayload>() : nullptr;
    const FTruongSinhResolvedActivityCommitPayload* ActivityPayload =
        Command.ActionId.Value == CommitResolvedActivityActionId ?
            Command.Payload.GetPtr<FTruongSinhResolvedActivityCommitPayload>() : nullptr;
    if (!AdvancePayload && !CultivationPayload && !ActivityPayload)
    {
        return Reject(InOutState, Command, TruongSinhSimulationIds::ReasonUnknownAction);
    }

    const int64 Minutes = AdvancePayload ? AdvancePayload->Minutes :
        CultivationPayload ? CultivationPayload->Minutes : ActivityPayload->Minutes;
    if (Minutes <= 0 || (CultivationPayload &&
        (CultivationPayload->CultivationProgressUnits < 0 || !CultivationPayload->OutcomeId.IsValid() ||
            !CultivationPayload->ReplayId.IsValid())) ||
        (ActivityPayload && (!ActivityPayload->ActivityId.IsValid() || !ActivityPayload->RequiredCurrentRealmId.IsValid() ||
            ActivityPayload->CultivationProgressUnits < 0 || ActivityPayload->RealmLifespanBonusDays < 0 ||
            (!ActivityPayload->NewRealmId.Value.IsEmpty() && !ActivityPayload->NewRealmId.IsValid()) ||
            !ActivityPayload->OutcomeId.IsValid() || !ActivityPayload->ReplayId.IsValid() ||
            ActivityPayload->OutputUnits < 0 || ActivityPayload->OutputQualityBps < 0 ||
            ActivityPayload->OutputQualityBps > 10000 || ActivityPayload->OutputImpurityBps < 0 ||
            ActivityPayload->OutputImpurityBps > 10000 ||
            (ActivityPayload->OutputUnits > 0 && !ActivityPayload->OutputId.IsValid()) ||
            (ActivityPayload->OutputUnits == 0 && (ActivityPayload->OutputId.IsValid() ||
                ActivityPayload->OutputQualityBps != 0 || ActivityPayload->OutputImpurityBps != 0)))))
    {
        return Reject(InOutState, Command, TruongSinhSimulationIds::ReasonInvalidPayload);
    }
    if (ActivityPayload && !(ActivityPayload->RequiredCurrentRealmId == InOutState.CurrentVessel.RealmId))
    {
        return Reject(InOutState, Command, TruongSinhSimulationIds::ReasonActivityPrecondition);
    }
    if (ActivityPayload && ActivityPayload->OutputUnits > 0 &&
        InOutState.ActivityOutputRecords.ContainsByPredicate([&Command](const FTruongSinhActivityOutputRecord& Record)
        {
            return Record.CommandId == Command.CommandId;
        }))
    {
        return Reject(InOutState, Command, TruongSinhSimulationIds::ReasonDuplicateCommand);
    }
    const int64 CultivationDelta = CultivationPayload ? CultivationPayload->CultivationProgressUnits :
        ActivityPayload ? ActivityPayload->CultivationProgressUnits : 0;
    const int64 RealmLifespanDelta = ActivityPayload ? ActivityPayload->RealmLifespanBonusDays : 0;
    if (InOutState.ElapsedMinutes > MAX_int64 - Minutes ||
        InOutState.CurrentVessel.CultivationUnits > MAX_int64 - CultivationDelta ||
        InOutState.CurrentVessel.Lifespan.RealmBonusDays > MAX_int64 - RealmLifespanDelta)
    {
        return Reject(InOutState, Command, TruongSinhSimulationIds::ReasonOverflow);
    }

    FTruongSinhActionResult Result;
    Result.Status = ETruongSinhActionStatus::Committed;
    Result.ActionId = Command.ActionId;
    Result.PreviousWorldRevision = InOutState.WorldRevision;

    const int64 NewElapsedMinutes = InOutState.ElapsedMinutes + Minutes;
    const int64 PreviousWholeDays = InOutState.ElapsedMinutes / 1440;
    const int64 NewWholeDays = NewElapsedMinutes / 1440;
    const int64 BiologicalDaysAdded = NewWholeDays - PreviousWholeDays;
    if (InOutState.CurrentVessel.Lifespan.BiologicalAgeDays > MAX_int64 - BiologicalDaysAdded)
    {
        return Reject(InOutState, Command, TruongSinhSimulationIds::ReasonOverflow);
    }
    InOutState.ElapsedMinutes = NewElapsedMinutes;
    InOutState.CurrentVessel.Lifespan.BiologicalAgeDays += BiologicalDaysAdded;
    InOutState.CurrentVessel.CultivationUnits += CultivationDelta;
    if (ActivityPayload)
    {
        InOutState.CurrentVessel.Lifespan.RealmBonusDays += RealmLifespanDelta;
        if (ActivityPayload->NewRealmId.IsValid())
        {
            InOutState.CurrentVessel.RealmId = ActivityPayload->NewRealmId;
        }
        if (ActivityPayload->OutputUnits > 0)
        {
            FTruongSinhActivityOutputRecord Record;
            Record.CommandId = Command.CommandId;
            Record.ActivityId = ActivityPayload->ActivityId;
            Record.OutputId = ActivityPayload->OutputId;
            Record.Units = ActivityPayload->OutputUnits;
            Record.QualityBps = ActivityPayload->OutputQualityBps;
            Record.ImpurityBps = ActivityPayload->OutputImpurityBps;
            InOutState.ActivityOutputRecords.Add(MoveTemp(Record));
        }
    }
    ++InOutState.WorldRevision;
    InOutState.CommittedCommandIds.Add(Command.CommandId);

    FTruongSinhDomainEvent Event;
    Event.EventTypeId.Value = CultivationPayload ? TruongSinhSimulationIds::EventCultivationCommitted :
        ActivityPayload ? TruongSinhSimulationIds::EventResolvedActivityCommitted :
        TruongSinhSimulationIds::EventTimeAdvanced;
    Event.Sequence = 0;
    if (CultivationPayload)
    {
        Event.Payload.InitializeAs<FTruongSinhCultivationCommitPayload>(*CultivationPayload);
    }
    else if (ActivityPayload)
    {
        Event.Payload.InitializeAs<FTruongSinhResolvedActivityCommitPayload>(*ActivityPayload);
    }
    else
    {
        Event.Payload.InitializeAs<FTruongSinhAdvanceTimePayload>(*AdvancePayload);
    }
    Result.Events.Add(MoveTemp(Event));
    Result.NewWorldRevision = InOutState.WorldRevision;
    Result.StateHash = ComputeStateHash(InOutState);
    return Result;
}

FString FTruongSinhGameSimulation::ComputeStateHash(const FTruongSinhSimulationState& State)
{
    TArray<FString> CommandIds;
    CommandIds.Reserve(State.CommittedCommandIds.Num());
    for (const FGuid& Id : State.CommittedCommandIds)
    {
        CommandIds.Add(Id.ToString(EGuidFormats::Digits));
    }
    CommandIds.Sort();

    TArray<FString> RngStreams;
    RngStreams.Reserve(State.Rng.Streams.Num());
    for (const FTruongSinhRngStreamState& Stream : State.Rng.Streams)
    {
        RngStreams.Add(FString::Printf(
            TEXT("%s:%016llx:%lld"),
            *Stream.StreamId,
            static_cast<unsigned long long>(Stream.State),
            static_cast<long long>(Stream.DrawCount)));
    }
    RngStreams.Sort();

    TArray<FString> ActivityOutputs;
    ActivityOutputs.Reserve(State.ActivityOutputRecords.Num());
    for (const FTruongSinhActivityOutputRecord& Record : State.ActivityOutputRecords)
    {
        ActivityOutputs.Add(FString::Printf(TEXT("%s:%s:%s:%lld:%d:%d"),
            *Record.CommandId.ToString(EGuidFormats::Digits), *Record.ActivityId.Value,
            *Record.OutputId.Value, static_cast<long long>(Record.Units), Record.QualityBps,
            Record.ImpurityBps));
    }
    ActivityOutputs.Sort();

    const auto SortedStableIds = [](const TArray<FTruongSinhStableId>& Ids)
    {
        TArray<FString> Values;
        Values.Reserve(Ids.Num());
        for (const FTruongSinhStableId& Id : Ids)
        {
            Values.Add(Id.Value);
        }
        Values.Sort();
        return Values;
    };

    const TArray<FString> KnownTechniques = SortedStableIds(State.Soul.KnownTechniqueIds);
    const TArray<FString> Memories = SortedStableIds(State.Soul.MemoryIds);
    const TArray<FString> VesselHistory = SortedStableIds(State.Soul.VesselHistoryIds);
    const TArray<FString> Relationships = SortedStableIds(State.CurrentVessel.RelationshipIds);
    const TArray<FString> OwnedAssets = SortedStableIds(State.CurrentVessel.OwnedAssetIds);

    const FString Canonical = FString::Printf(
        TEXT("schema=%d|minutes=%lld|remainder=%lld|revision=%lld|layer=%s|soul=%s:%d:%s:%s:%s|vessel=%s:%s:%s:%s:%s:%lld:%lld:%lld:%lld:%lld:%lld:%lld:%s:%s|rng=%d:%lld|streams=%s|commands=%s|outputs=%s"),
        State.SchemaVersion,
        static_cast<long long>(State.ElapsedMinutes),
        static_cast<long long>(State.ExplorationRemainderMillis),
        static_cast<long long>(State.WorldRevision),
        *State.WorldLayerId.Value,
        *State.Soul.SoulId.Value,
        State.Soul.IntegrityUnits,
        *FString::Join(KnownTechniques, TEXT(",")),
        *FString::Join(Memories, TEXT(",")),
        *FString::Join(VesselHistory, TEXT(",")),
        *State.CurrentVessel.VesselId.Value,
        *State.CurrentVessel.IdentityId.Value,
        *State.CurrentVessel.SpiritualRootId.Value,
        *State.CurrentVessel.RealmId.Value,
        *State.CurrentVessel.SectId.Value,
        static_cast<long long>(State.CurrentVessel.CultivationUnits),
        static_cast<long long>(State.CurrentVessel.Lifespan.BiologicalAgeDays),
        static_cast<long long>(State.CurrentVessel.Lifespan.BaseLifespanDays),
        static_cast<long long>(State.CurrentVessel.Lifespan.RealmBonusDays),
        static_cast<long long>(State.CurrentVessel.Lifespan.TechniqueBonusDays),
        static_cast<long long>(State.CurrentVessel.Lifespan.PillAndResourceBonusDays),
        static_cast<long long>(State.CurrentVessel.Lifespan.PermanentDamageDays),
        *FString::Join(Relationships, TEXT(",")),
        *FString::Join(OwnedAssets, TEXT(",")),
        State.Rng.AlgorithmVersion,
        static_cast<long long>(State.Rng.MasterSeed),
        *FString::Join(RngStreams, TEXT(",")),
        *FString::Join(CommandIds, TEXT(",")),
        *FString::Join(ActivityOutputs, TEXT(",")));

    FTCHARToUTF8 Utf8(*Canonical);
    const FBlake3Hash Hash = FBlake3::HashBuffer(Utf8.Get(), static_cast<uint64>(Utf8.Length()));
    return FString::Printf(TEXT("blake3-v1:%s"), *LexToString(Hash));
}

FTruongSinhActionResult FTruongSinhGameSimulation::Reject(
    const FTruongSinhSimulationState& State,
    const FTruongSinhActionCommand& Command,
    const TCHAR* ReasonId)
{
    FTruongSinhActionResult Result;
    Result.Status = ETruongSinhActionStatus::Rejected;
    Result.ActionId = Command.ActionId;
    Result.ReasonId.Value = ReasonId;
    Result.PreviousWorldRevision = State.WorldRevision;
    Result.NewWorldRevision = State.WorldRevision;
    return Result;
}

void UTruongSinhGameSimulationFacade::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    State = FTruongSinhGameSimulation::CreateNewGame(0);
}

void UTruongSinhGameSimulationFacade::StartNewGame(const int64 MasterSeed)
{
    State = FTruongSinhGameSimulation::CreateNewGame(MasterSeed);
}

FTruongSinhActionResult UTruongSinhGameSimulationFacade::Execute(const FTruongSinhActionCommand& Command)
{
    return FTruongSinhGameSimulation::Execute(State, Command);
}

FTruongSinhSimulationState UTruongSinhGameSimulationFacade::GetState() const
{
    return State;
}

void UTruongSinhGameSimulationFacade::RestoreFromSave(const FTruongSinhSimulationState& LoadedState)
{
    State = LoadedState;
}
