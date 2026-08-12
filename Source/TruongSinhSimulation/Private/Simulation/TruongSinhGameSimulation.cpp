#include "Simulation/TruongSinhGameSimulation.h"

#include "Containers/StringConv.h"
#include "Hash/Blake3.h"

namespace TruongSinhSimulationIds
{
constexpr TCHAR EventTimeAdvanced[] = TEXT("core.time_advanced");
constexpr TCHAR ReasonDuplicateCommand[] = TEXT("core.reject.duplicate_command");
constexpr TCHAR ReasonInvalidCommand[] = TEXT("core.reject.invalid_command");
constexpr TCHAR ReasonInvalidPayload[] = TEXT("core.reject.invalid_payload");
constexpr TCHAR ReasonOverflow[] = TEXT("core.reject.overflow");
constexpr TCHAR ReasonRevisionMismatch[] = TEXT("core.reject.revision_mismatch");
constexpr TCHAR ReasonUnknownAction[] = TEXT("core.reject.unknown_action");
}

const TCHAR* FTruongSinhGameSimulation::AdvanceTimeActionId = TEXT("core.advance_time");

FTruongSinhSimulationState FTruongSinhGameSimulation::CreateNewGame(const int64 MasterSeed)
{
    FTruongSinhSimulationState State;
    State.Rng.MasterSeed = MasterSeed;
    State.Rng.AlgorithmVersion = 1;
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
    if (Command.ActionId.Value != AdvanceTimeActionId)
    {
        return Reject(InOutState, Command, TruongSinhSimulationIds::ReasonUnknownAction);
    }

    const FTruongSinhAdvanceTimePayload* Payload = Command.Payload.GetPtr<FTruongSinhAdvanceTimePayload>();
    if (!Payload || Payload->DayCount <= 0)
    {
        return Reject(InOutState, Command, TruongSinhSimulationIds::ReasonInvalidPayload);
    }
    if (InOutState.ElapsedDays > MAX_int64 - static_cast<int64>(Payload->DayCount))
    {
        return Reject(InOutState, Command, TruongSinhSimulationIds::ReasonOverflow);
    }

    FTruongSinhActionResult Result;
    Result.Status = ETruongSinhActionStatus::Committed;
    Result.ActionId = Command.ActionId;
    Result.PreviousWorldRevision = InOutState.WorldRevision;

    InOutState.ElapsedDays += Payload->DayCount;
    ++InOutState.WorldRevision;
    InOutState.CommittedCommandIds.Add(Command.CommandId);

    FTruongSinhDomainEvent Event;
    Event.EventTypeId.Value = TruongSinhSimulationIds::EventTimeAdvanced;
    Event.Sequence = 0;
    Event.Payload.InitializeAs<FTruongSinhAdvanceTimePayload>(*Payload);
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

    const FString Canonical = FString::Printf(
        TEXT("schema=%d|days=%lld|revision=%lld|rng=%d:%lld|streams=%s|commands=%s"),
        State.SchemaVersion,
        static_cast<long long>(State.ElapsedDays),
        static_cast<long long>(State.WorldRevision),
        State.Rng.AlgorithmVersion,
        static_cast<long long>(State.Rng.MasterSeed),
        *FString::Join(RngStreams, TEXT(",")),
        *FString::Join(CommandIds, TEXT(",")));

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
