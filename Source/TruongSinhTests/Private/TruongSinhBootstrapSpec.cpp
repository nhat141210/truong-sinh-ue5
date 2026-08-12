#if WITH_DEV_AUTOMATION_TESTS

#include "Core/TruongSinhTypes.h"
#include "Core/TruongSinhDeterministicRng.h"
#include "Data/TruongSinhActivityRegistryDataAsset.h"
#include "GameFramework/InputSettings.h"
#include "Misc/AutomationTest.h"
#include "Resolution/TruongSinhActivityResolution.h"
#include "Save/TruongSinhSaveGameV2.h"
#include "Simulation/TruongSinhGameSimulation.h"
#include "Simulation/TruongSinhLifeState.h"
#include "TruongSinhInteractionProvider.h"

namespace
{
FTruongSinhActionCommand MakeAdvanceTimeCommand(
    const FGuid CommandId,
    const int64 ExpectedRevision,
    const int64 Minutes)
{
    FTruongSinhActionCommand Command;
    Command.CommandId = CommandId;
    Command.ActionId.Value = FTruongSinhGameSimulation::AdvanceTimeActionId;
    Command.InstigatorId.Value = TEXT("player.main");
    Command.ExpectedWorldRevision = ExpectedRevision;
    Command.Sequence = ExpectedRevision;

    FTruongSinhAdvanceTimePayload Payload;
    Payload.Minutes = Minutes;
    Command.Payload.InitializeAs<FTruongSinhAdvanceTimePayload>(Payload);
    return Command;
}
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FTruongSinhStableIdSpec,
    "TruongSinh.Core.StableId",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FTruongSinhStableIdSpec::RunTest(const FString& Parameters)
{
    FTruongSinhStableId ValidId;
    ValidId.Value = TEXT("technique.example");
    TestTrue(TEXT("A non-empty stable identifier is valid"), ValidId.IsValid());

    FTruongSinhStableId EmptyId;
    TestFalse(TEXT("An empty stable identifier is invalid"), EmptyId.IsValid());

    FTruongSinhStableId UppercaseId;
    UppercaseId.Value = TEXT("Technique.Example");
    TestFalse(TEXT("Stable identifiers must use canonical lowercase ASCII"), UppercaseId.IsValid());

    FTruongSinhStableId SpacedId;
    SpacedId.Value = TEXT("technique example");
    TestFalse(TEXT("Stable identifiers cannot contain spaces"), SpacedId.IsValid());
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FTruongSinhActivityRegistrySpec,
    "TruongSinh.Data.ActivityRegistry",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FTruongSinhActivityRegistrySpec::RunTest(const FString& Parameters)
{
    UTruongSinhActivityRegistryDataAsset* Registry = NewObject<UTruongSinhActivityRegistryDataAsset>();
    FTruongSinhActivityDefinition Cultivation;
    Cultivation.ActivityId.Value = TEXT("activity.cultivation.test");
    Cultivation.FacilityId.Value = TEXT("facility.cultivation.test");
    Cultivation.MethodId.Value = TEXT("method.test");
    Cultivation.LocationId.Value = TEXT("location.test");
    Cultivation.ResolverId = TEXT("cultivation");
    Cultivation.DurationMinutes = 480;
    Cultivation.DifficultyOrTargetPower = 6500;
    Registry->Definitions.Add(Cultivation);

    FString Error;
    TestTrue(TEXT("A complete activity definition validates"), Registry->ValidateRegistry(Error));
    const FTruongSinhActivityDefinition* Found = Registry->FindByFacility(Cultivation.FacilityId);
    TestNotNull(TEXT("Facility lookup returns the authored definition"), Found);
    if (Found)
    {
        TestEqual(TEXT("Lookup preserves resolver registration"), Found->ResolverId, FName(TEXT("cultivation")));
        TestEqual(TEXT("Lookup preserves exact duration"), Found->DurationMinutes, 480ll);
    }

    FTruongSinhActivityDefinition Duplicate = Cultivation;
    Duplicate.ActivityId.Value = TEXT("activity.cultivation.duplicate");
    Registry->Definitions.Add(Duplicate);
    TestFalse(TEXT("Duplicate facility IDs are rejected before runtime"), Registry->ValidateRegistry(Error));
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FTruongSinhMouseCaptureDefaultsSpec,
    "TruongSinh.Input.MouseCaptureDefaults",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FTruongSinhMouseCaptureDefaultsSpec::RunTest(const FString& Parameters)
{
    const UInputSettings* InputSettings = GetDefault<UInputSettings>();
    TestNotNull(TEXT("Input settings are available"), InputSettings);
    if (!InputSettings)
    {
        return false;
    }

    TestTrue(TEXT("Game captures the mouse when the viewport launches"),
        static_cast<bool>(InputSettings->bCaptureMouseOnLaunch));
    TestEqual(TEXT("Gameplay capture includes the initial mouse down"),
        InputSettings->DefaultViewportMouseCaptureMode,
        EMouseCaptureMode::CapturePermanently_IncludingInitialMouseDown);
    TestEqual(TEXT("Captured mouse remains locked to the viewport"),
        InputSettings->DefaultViewportMouseLockMode,
        EMouseLockMode::LockOnCapture);
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FTruongSinhInteractionSelectionSpec,
    "TruongSinh.World.InteractionSelection",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FTruongSinhInteractionSelectionSpec::RunTest(const FString& Parameters)
{
    FTruongSinhInteractionOffer DisabledHighPriority;
    DisabledHighPriority.CandidateId.Value = TEXT("interaction.disabled");
    DisabledHighPriority.Priority = 1000;
    DisabledHighPriority.bEnabled = false;

    FTruongSinhInteractionOffer OutOfRange;
    OutOfRange.CandidateId.Value = TEXT("interaction.too_far");
    OutOfRange.Priority = 500;
    OutOfRange.MaximumRangeCentimeters = 199.0f;

    FTruongSinhInteractionOffer StableTieB;
    StableTieB.CandidateId.Value = TEXT("interaction.tie_b");
    StableTieB.Priority = 100;
    StableTieB.MaximumRangeCentimeters = 200.0f;

    FTruongSinhInteractionOffer StableTieA = StableTieB;
    StableTieA.CandidateId.Value = TEXT("interaction.tie_a");

    const TArray<FTruongSinhInteractionOffer> Offers =
        {DisabledHighPriority, OutOfRange, StableTieB, StableTieA};
    FTruongSinhInteractionOffer Selected;
    TestTrue(TEXT("An enabled offer at the exact range boundary is discoverable"),
        FTruongSinhInteractionSelection::SelectBestOffer(Offers, 200.0f, Selected));
    TestEqual(TEXT("Disabled and out-of-range offers are ignored; stable ID breaks priority ties"),
        Selected.CandidateId.Value, FString(TEXT("interaction.tie_a")));

    Selected = StableTieA;
    TestFalse(TEXT("Negative distances are rejected"),
        FTruongSinhInteractionSelection::SelectBestOffer(Offers, -1.0f, Selected));
    TestFalse(TEXT("A failed selection clears stale output"), Selected.CandidateId.IsValid());
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FTruongSinhDeterministicRngGoldenVectorSpec,
    "TruongSinh.Core.DeterministicRng.GoldenVectorV1",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FTruongSinhDeterministicRngGoldenVectorSpec::RunTest(const FString& Parameters)
{
    FTruongSinhRngStreamState Stream =
        FTruongSinhDeterministicRng::CreateStream(123456789, TEXT("world.events"));

    TestEqual(TEXT("Draw 1"), FTruongSinhDeterministicRng::NextUInt64(Stream), 0xA6DAC671FE8C56E6ull);
    TestEqual(TEXT("Draw 2"), FTruongSinhDeterministicRng::NextUInt64(Stream), 0x19EB9E31DE5BA92Aull);
    TestEqual(TEXT("Draw 3"), FTruongSinhDeterministicRng::NextUInt64(Stream), 0x0947A77A07217EC4ull);
    TestEqual(TEXT("Draw count"), Stream.DrawCount, 3ll);
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FTruongSinhDeterministicRngStreamIsolationSpec,
    "TruongSinh.Core.DeterministicRng.StreamIsolation",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FTruongSinhDeterministicRngStreamIsolationSpec::RunTest(const FString& Parameters)
{
    FTruongSinhRngStreamState WorldStream =
        FTruongSinhDeterministicRng::CreateStream(42, TEXT("world.events"));
    FTruongSinhRngStreamState ActivityStream =
        FTruongSinhDeterministicRng::CreateStream(42, TEXT("activity.resolve"));

    TestNotEqual(
        TEXT("Stable stream IDs derive independent sequences"),
        FTruongSinhDeterministicRng::NextUInt64(WorldStream),
        FTruongSinhDeterministicRng::NextUInt64(ActivityStream));

    const int32 Roll = FTruongSinhDeterministicRng::NextIntInclusive(WorldStream, 3, 7);
    TestTrue(TEXT("Inclusive bounded draw remains in range"), Roll >= 3 && Roll <= 7);
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FTruongSinhCanonicalCommandSpec,
    "TruongSinh.Simulation.CanonicalCommand",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FTruongSinhCanonicalCommandSpec::RunTest(const FString& Parameters)
{
    FTruongSinhSimulationState State = FTruongSinhGameSimulation::CreateNewGame(141210);
    const FGuid CommandId(1, 2, 3, 4);
    const FTruongSinhActionCommand Command = MakeAdvanceTimeCommand(CommandId, 0, 7 * 1440);

    const FTruongSinhActionResult First = FTruongSinhGameSimulation::Execute(State, Command);
    TestEqual(TEXT("Valid action commits"), First.Status, ETruongSinhActionStatus::Committed);
    TestEqual(TEXT("Time advances only by command payload"), State.ElapsedMinutes, 7ll * 1440ll);
    TestEqual(TEXT("Biological age advances at whole-day boundaries"),
        State.CurrentVessel.Lifespan.BiologicalAgeDays, 18ll * 365ll + 7ll);
    TestEqual(TEXT("Revision advances once"), State.WorldRevision, 1ll);
    TestEqual(TEXT("One ordered event is emitted"), First.Events.Num(), 1);
    TestFalse(TEXT("Committed state has a hash"), First.StateHash.IsEmpty());

    const FString HashAfterFirst = FTruongSinhGameSimulation::ComputeStateHash(State);
    FTruongSinhSimulationState DifferentBonuses = State;
    ++DifferentBonuses.CurrentVessel.Lifespan.PillAndResourceBonusDays;
    ++DifferentBonuses.CurrentVessel.Lifespan.PermanentDamageDays;
    TestNotEqual(TEXT("Hash preserves bonus and damage provenance even when the net lifespan is unchanged"),
        FTruongSinhGameSimulation::ComputeStateHash(DifferentBonuses), HashAfterFirst);
    const FTruongSinhActionResult Duplicate = FTruongSinhGameSimulation::Execute(State, Command);
    TestEqual(TEXT("Duplicate command is rejected"), Duplicate.Status, ETruongSinhActionStatus::Rejected);
    TestEqual(TEXT("Duplicate command cannot charge time twice"), State.ElapsedMinutes, 7ll * 1440ll);
    TestEqual(TEXT("Duplicate command cannot advance revision"), State.WorldRevision, 1ll);
    TestEqual(TEXT("Rejected command leaves hash unchanged"), FTruongSinhGameSimulation::ComputeStateHash(State), HashAfterFirst);

    const FTruongSinhActionCommand Stale = MakeAdvanceTimeCommand(FGuid(5, 6, 7, 8), 0, 2);
    const FTruongSinhActionResult StaleResult = FTruongSinhGameSimulation::Execute(State, Stale);
    TestEqual(TEXT("Stale revision is rejected"), StaleResult.Status, ETruongSinhActionStatus::Rejected);
    TestEqual(TEXT("Stale command leaves time unchanged"), State.ElapsedMinutes, 7ll * 1440ll);
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FTruongSinhDeterministicCommandReplaySpec,
    "TruongSinh.Simulation.DeterministicReplay",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FTruongSinhDeterministicCommandReplaySpec::RunTest(const FString& Parameters)
{
    FTruongSinhSimulationState Left = FTruongSinhGameSimulation::CreateNewGame(20260812);
    FTruongSinhSimulationState Right = FTruongSinhGameSimulation::CreateNewGame(20260812);
    const FTruongSinhActionCommand Command = MakeAdvanceTimeCommand(FGuid(11, 12, 13, 14), 0, 30 * 1440);

    const FTruongSinhActionResult LeftResult = FTruongSinhGameSimulation::Execute(Left, Command);
    const FTruongSinhActionResult RightResult = FTruongSinhGameSimulation::Execute(Right, Command);
    TestEqual(TEXT("Same seed and command produce same result hash"), LeftResult.StateHash, RightResult.StateHash);
    TestEqual(TEXT("Same input produces same elapsed minutes"), Left.ElapsedMinutes, Right.ElapsedMinutes);
    TestEqual(TEXT("Same input produces same revision"), Left.WorldRevision, Right.WorldRevision);
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FTruongSinhSaveRoundTripSpec,
    "TruongSinh.Save.RoundTripV2",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FTruongSinhSaveRoundTripSpec::RunTest(const FString& Parameters)
{
    FTruongSinhSaveGameV2 Save;
    Save.GameBuildId = TEXT("automation-test");
    Save.Simulation = FTruongSinhGameSimulation::CreateNewGame(987654321);

    FTruongSinhRngStreamState Stream =
        FTruongSinhDeterministicRng::CreateStream(Save.Simulation.Rng.MasterSeed, TEXT("world.events"));
    FTruongSinhDeterministicRng::NextUInt64(Stream);
    Save.Simulation.Rng.Streams.Add(Stream);

    FTruongSinhStableId Technique;
    Technique.Value = TEXT("technique.breathing.sample");
    Save.Simulation.Soul.KnownTechniqueIds.Add(Technique);
    const FTruongSinhActionCommand Command = MakeAdvanceTimeCommand(FGuid(21, 22, 23, 24), 0, 9 * 1440);
    FTruongSinhGameSimulation::Execute(Save.Simulation, Command);
    Save.PayloadHash = FTruongSinhGameSimulation::ComputeStateHash(Save.Simulation);

    FString Json;
    FString Error;
    TestTrue(TEXT("Save v2 serializes"), FTruongSinhSaveJsonCodecV2::Serialize(Save, Json, Error));
    if (!Error.IsEmpty())
    {
        AddError(Error);
    }

    FTruongSinhSaveGameV2 Loaded;
    const bool bLoaded = FTruongSinhSaveJsonCodecV2::Deserialize(Json, Loaded, Error);
    TestTrue(TEXT("Save v2 deserializes"), bLoaded);
    if (!Error.IsEmpty())
    {
        AddError(Error);
    }
    if (!bLoaded || Loaded.Simulation.Rng.Streams.IsEmpty())
    {
        return false;
    }
    TestEqual(TEXT("Round-trip preserves state hash"),
        FTruongSinhGameSimulation::ComputeStateHash(Loaded.Simulation), Save.PayloadHash);
    TestEqual(TEXT("Round-trip preserves RNG bits"), Loaded.Simulation.Rng.Streams[0].State, Stream.State);
    TestEqual(TEXT("Round-trip preserves command IDs"), Loaded.Simulation.CommittedCommandIds.Num(), 1);
    TestEqual(TEXT("Round-trip preserves soul knowledge"), Loaded.Simulation.Soul.KnownTechniqueIds.Num(), 1);

    FString CorruptJson = Json;
    const FString HashNeedle = FString::Printf(TEXT("\"%s\""), *Save.PayloadHash);
    TestTrue(TEXT("Serialized payload contains its canonical hash"), CorruptJson.Contains(HashNeedle));
    CorruptJson = CorruptJson.Replace(
        *HashNeedle,
        TEXT("\"blake3-v1:0000000000000000000000000000000000000000000000000000000000000000\""));
    FTruongSinhSaveGameV2 Corrupt;
    TestFalse(TEXT("Payload tampering fails checksum"),
        FTruongSinhSaveJsonCodecV2::Deserialize(CorruptJson, Corrupt, Error));
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FTruongSinhAutoResolutionSpec,
    "TruongSinh.Resolution.DeterministicAutoActivity",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FTruongSinhAutoResolutionSpec::RunTest(const FString& Parameters)
{
    FTruongSinhActivityPlan Plan;
    Plan.Action.CommandId = FGuid(31, 32, 33, 34);
    Plan.Action.ActionId.Value = TEXT("activity.resolve");
    Plan.Action.InstigatorId.Value = TEXT("player.main");
    Plan.ActivityId.Value = TEXT("activity.conflict.sample");
    Plan.MethodId.Value = TEXT("technique.sample");
    Plan.LocationId.Value = TEXT("location.arena.sample");
    Plan.Type = ETruongSinhActivityType::Conflict;
    Plan.Strategy = ETruongSinhActivityStrategy::Cautious;
    Plan.DurationMinutes = 30;

    FTruongSinhActivitySnapshot Strong;
    Strong.PerformerPower = 20000;
    Strong.DifficultyOrTargetPower = 10000;
    Strong.TechniqueModifierUnits = 500;
    Strong.MasterSeed = 141210;

    const FTruongSinhAutoResolutionResult First = FTruongSinhAutoResolver::Resolve(Strong, Plan);
    const FTruongSinhAutoResolutionResult Replay = FTruongSinhAutoResolver::Resolve(Strong, Plan);
    TestEqual(TEXT("Same plan and seed produce the same score"), First.FinalScore, Replay.FinalScore);
    TestEqual(TEXT("Large power lead cannot be overturned by bounded variation"),
        First.Outcome, ETruongSinhResolutionOutcome::GreatSuccess);
    TestEqual(TEXT("Resolution emits three reusable presentation beats"), First.Beats.Num(), 3);

    FTruongSinhActivitySnapshot Weak = Strong;
    Weak.PerformerPower = 1000;
    Weak.DifficultyOrTargetPower = 20000;
    const FTruongSinhAutoResolutionResult WeakResult = FTruongSinhAutoResolver::Resolve(Weak, Plan);
    TestEqual(TEXT("Large power deficit cannot win through RNG"),
        WeakResult.Outcome, ETruongSinhResolutionOutcome::Failure);
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FTruongSinhLifespanSpec,
    "TruongSinh.Life.LifespanMonotonicity",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FTruongSinhLifespanSpec::RunTest(const FString& Parameters)
{
    FTruongSinhLifespanState Life;
    Life.BiologicalAgeDays = 60 * 365;
    Life.BaseLifespanDays = 70 * 365;
    const int64 BaseRemaining = Life.RemainingLifespanDays();
    Life.RealmBonusDays = 100 * 365;
    TestTrue(TEXT("Realm breakthrough increases remaining lifespan"),
        Life.RemainingLifespanDays() > BaseRemaining);
    Life.TechniqueBonusDays = 20 * 365;
    Life.PillAndResourceBonusDays = 10 * 365;
    TestEqual(TEXT("Technique and resources add exact authored days"),
        Life.RemainingLifespanDays(), BaseRemaining + 130ll * 365ll);
    Life.PermanentDamageDays = 5 * 365;
    TestEqual(TEXT("Permanent lifespan damage is deducted once"),
        Life.RemainingLifespanDays(), BaseRemaining + 125ll * 365ll);
    Life.BaseLifespanDays = MAX_int64;
    Life.RealmBonusDays = MAX_int64;
    TestEqual(TEXT("Extreme authored values saturate instead of overflowing"),
        Life.EffectiveLifespanDays(), MAX_int64);
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FTruongSinhPossessionSpec,
    "TruongSinh.Soul.PossessionIdentityAndFallback",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FTruongSinhPossessionSpec::RunTest(const FString& Parameters)
{
    FTruongSinhSoulState Soul = FTruongSinhLifeRules::CreateInitialSoul();
    FTruongSinhStableId KnownTechnique;
    KnownTechnique.Value = TEXT("technique.soul.sample");
    Soul.KnownTechniqueIds.Add(KnownTechnique);
    const FTruongSinhVesselState Current = FTruongSinhLifeRules::CreateInitialVessel();

    FTruongSinhVesselState Target = Current;
    Target.VesselId.Value = TEXT("vessel.target.sample");
    Target.IdentityId.Value = TEXT("identity.target.sample");
    Target.RealmId.Value = TEXT("realm.foundation");
    Target.CultivationUnits = 9000;
    FTruongSinhStableId Asset;
    Asset.Value = TEXT("asset.target.home");
    Target.OwnedAssetIds.Add(Asset);
    FTruongSinhStableId Relation;
    Relation.Value = TEXT("relation.target.family");
    Target.RelationshipIds.Add(Relation);

    FTruongSinhPossessionRequest SuccessRequest;
    SuccessRequest.CommandId = FGuid(41, 42, 43, 44);
    SuccessRequest.PossessionTechniqueId.Value = TEXT("technique.possession.sample");
    SuccessRequest.MediumId.Value = TEXT("item.possession.medium");
    SuccessRequest.CompatibilityBps = 10000;
    SuccessRequest.MasterSeed = 141210;
    const FTruongSinhPossessionResult Success =
        FTruongSinhLifeRules::ResolvePossession(Soul, Current, Target, SuccessRequest);
    TestEqual(TEXT("Fully compatible possession succeeds"),
        Success.Outcome, ETruongSinhPossessionOutcome::Succeeded);
    TestEqual(TEXT("Target identity is inherited"), Success.Vessel.IdentityId.Value, Target.IdentityId.Value);
    TestEqual(TEXT("Target property is inherited"), Success.Vessel.OwnedAssetIds.Num(), 1);
    TestEqual(TEXT("Target relationships are inherited"), Success.Vessel.RelationshipIds.Num(), 1);
    TestEqual(TEXT("New body cultivation restarts"), Success.Vessel.CultivationUnits, 0ll);
    TestEqual(TEXT("Soul techniques survive possession"), Success.Soul.KnownTechniqueIds.Num(), 1);

    FTruongSinhPossessionRequest FailureRequest = SuccessRequest;
    FailureRequest.CommandId = FGuid(51, 52, 53, 54);
    FailureRequest.CompatibilityBps = 0;
    FailureRequest.bCurrentBodyUnavailable = true;
    FTruongSinhSoulState FragileSoul = Soul;
    FragileSoul.IntegrityUnits = 1;
    const FTruongSinhPossessionResult Fallback =
        FTruongSinhLifeRules::ResolvePossession(FragileSoul, Current, Target, FailureRequest);
    TestEqual(TEXT("Lost body produces emergency vessel instead of game over"),
        Fallback.Outcome, ETruongSinhPossessionOutcome::EmergencyVessel);
    TestTrue(TEXT("Emergency vessel is playable"), Fallback.Vessel.VesselId.IsValid());
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FTruongSinhCultivationCommitSpec,
    "TruongSinh.GoldenLoop.CultivationCommitAndReplay",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FTruongSinhCultivationCommitSpec::RunTest(const FString& Parameters)
{
    FTruongSinhSimulationState State = FTruongSinhGameSimulation::CreateNewGame(141210);
    FTruongSinhActivityPlan Plan;
    Plan.Action.CommandId = FGuid(61, 62, 63, 64);
    Plan.Action.ActionId.Value = FTruongSinhGameSimulation::CommitCultivationActionId;
    Plan.Action.InstigatorId = State.CurrentVessel.VesselId;
    Plan.Action.ExpectedWorldRevision = State.WorldRevision;
    Plan.Action.Sequence = 0;
    Plan.Type = ETruongSinhActivityType::Cultivation;
    Plan.ActivityId.Value = TEXT("activity.cultivation.test");
    Plan.MethodId.Value = TEXT("method.test");
    Plan.FacilityId.Value = TEXT("facility.test");
    Plan.LocationId.Value = TEXT("location.test");
    Plan.DurationMinutes = 480;

    FTruongSinhActivitySnapshot Snapshot;
    Snapshot.PerformerPower = 7200;
    Snapshot.DifficultyOrTargetPower = 6500;
    Snapshot.MasterSeed = State.Rng.MasterSeed;
    const FTruongSinhAutoResolutionResult Resolved = FTruongSinhAutoResolver::Resolve(Snapshot, Plan);
    TestTrue(TEXT("Resolved cultivation has stable replay ID"), Resolved.ReplayId.IsValid());

    FTruongSinhCultivationCommitPayload Payload;
    Payload.Minutes = Resolved.TimeAdvancedMinutes;
    Payload.CultivationProgressUnits = Resolved.CultivationProgressUnits;
    Payload.OutcomeId = Resolved.OutcomeId;
    Payload.ReplayId = Resolved.ReplayId;
    Plan.Action.Payload.InitializeAs<FTruongSinhCultivationCommitPayload>(Payload);

    const FTruongSinhActionResult First = FTruongSinhGameSimulation::Execute(State, Plan.Action);
    TestEqual(TEXT("Cultivation commits"), First.Status, ETruongSinhActionStatus::Committed);
    TestEqual(TEXT("Cultivation advances exact time"), State.ElapsedMinutes, 480ll);
    TestEqual(TEXT("Cultivation progress is canonical"), State.CurrentVessel.CultivationUnits,
        Resolved.CultivationProgressUnits);
    const int64 ProgressAfterCommit = State.CurrentVessel.CultivationUnits;
    const FTruongSinhActionResult Duplicate = FTruongSinhGameSimulation::Execute(State, Plan.Action);
    TestEqual(TEXT("Duplicate command is rejected"), Duplicate.Status, ETruongSinhActionStatus::Rejected);
    TestEqual(TEXT("Duplicate does not apply progress twice"), State.CurrentVessel.CultivationUnits,
        ProgressAfterCommit);

    FTruongSinhSaveGameV2 Save;
    Save.Simulation = State;
    Save.PayloadHash = FTruongSinhGameSimulation::ComputeStateHash(State);
    Save.PendingReplayId = Resolved.ReplayId;
    FString Json;
    FString Error;
    TestTrue(TEXT("Golden loop save serializes"), FTruongSinhSaveJsonCodecV2::Serialize(Save, Json, Error));
    FTruongSinhSaveGameV2 Loaded;
    TestTrue(TEXT("Golden loop save reloads"), FTruongSinhSaveJsonCodecV2::Deserialize(Json, Loaded, Error));
    TestEqual(TEXT("Pending replay survives Continue"), Loaded.PendingReplayId.Value, Resolved.ReplayId.Value);
    TestEqual(TEXT("Reload keeps committed progress"), Loaded.Simulation.CurrentVessel.CultivationUnits,
        ProgressAfterCommit);
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FTruongSinhBreakthroughCommitSpec,
    "TruongSinh.Activity.BreakthroughCommitAndLifespan",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FTruongSinhBreakthroughCommitSpec::RunTest(const FString& Parameters)
{
    FTruongSinhSimulationState State = FTruongSinhGameSimulation::CreateNewGame(141210);
    FTruongSinhActivityPlan Plan;
    Plan.Action.CommandId = FGuid(71, 72, 73, 74);
    Plan.Action.ActionId.Value = FTruongSinhGameSimulation::CommitResolvedActivityActionId;
    Plan.Action.InstigatorId = State.CurrentVessel.VesselId;
    Plan.Action.ExpectedWorldRevision = State.WorldRevision;
    Plan.Action.Sequence = 0;
    Plan.Type = ETruongSinhActivityType::Breakthrough;
    Plan.ActivityId.Value = TEXT("activity.breakthrough.foundation");
    Plan.MethodId.Value = TEXT("method.five_elements_breathing");
    Plan.FacilityId.Value = TEXT("facility.breakthrough.dev_smoke");
    Plan.LocationId.Value = TEXT("zone.lower_realm.dev_smoke");
    Plan.DurationMinutes = 720;
    Plan.Strategy = ETruongSinhActivityStrategy::Balanced;

    FTruongSinhActivitySnapshot Snapshot;
    Snapshot.PerformerPower = 10000;
    Snapshot.DifficultyOrTargetPower = 6500;
    Snapshot.TechniqueModifierUnits = 350;
    Snapshot.PreparationModifierUnits = 300;
    Snapshot.EnvironmentModifierUnits = 450;
    Snapshot.MasterSeed = State.Rng.MasterSeed;
    const FTruongSinhAutoResolutionResult Resolution = FTruongSinhAutoResolver::Resolve(Snapshot, Plan);
    TestEqual(TEXT("Breakthrough is a deterministic great success at a large score gap"),
        Resolution.Outcome, ETruongSinhResolutionOutcome::GreatSuccess);
    TestEqual(TEXT("Breakthrough emits the foundation realm"),
        Resolution.NewRealmId.Value, FString(TEXT("realm.foundation")));
    TestEqual(TEXT("Great breakthrough grants authored lifespan"),
        Resolution.RealmLifespanBonusDays, 20ll * 365ll);

    FTruongSinhResolvedActivityCommitPayload Payload;
    Payload.ActivityId = Plan.ActivityId;
    Payload.RequiredCurrentRealmId = State.CurrentVessel.RealmId;
    Payload.Minutes = Resolution.TimeAdvancedMinutes;
    Payload.CultivationProgressUnits = Resolution.CultivationProgressUnits;
    Payload.RealmLifespanBonusDays = Resolution.RealmLifespanBonusDays;
    Payload.NewRealmId = Resolution.NewRealmId;
    Payload.OutcomeId = Resolution.OutcomeId;
    Payload.ReplayId = Resolution.ReplayId;
    Plan.Action.Payload.InitializeAs<FTruongSinhResolvedActivityCommitPayload>(Payload);

    const int64 RemainingBefore = State.CurrentVessel.Lifespan.RemainingLifespanDays();
    const FTruongSinhActionResult Commit = FTruongSinhGameSimulation::Execute(State, Plan.Action);
    TestEqual(TEXT("Resolved breakthrough commits through the generic activity gateway"),
        Commit.Status, ETruongSinhActionStatus::Committed);
    TestEqual(TEXT("Breakthrough changes canonical realm"),
        State.CurrentVessel.RealmId.Value, FString(TEXT("realm.foundation")));
    TestEqual(TEXT("Breakthrough retains its full realm lifespan contribution"),
        State.CurrentVessel.Lifespan.RealmBonusDays, 20ll * 365ll);
    TestEqual(TEXT("Breakthrough increases remaining lifespan by its authored bonus"),
        State.CurrentVessel.Lifespan.RemainingLifespanDays(), RemainingBefore + 20ll * 365ll);
    TestEqual(TEXT("Generic activity advances its exact planned time"), State.ElapsedMinutes, 720ll);

    const FTruongSinhActionResult Duplicate = FTruongSinhGameSimulation::Execute(State, Plan.Action);
    TestEqual(TEXT("Breakthrough retry cannot grant lifespan twice"),
        Duplicate.Status, ETruongSinhActionStatus::Rejected);
    TestEqual(TEXT("Duplicate leaves realm bonus unchanged"),
        State.CurrentVessel.Lifespan.RealmBonusDays, 20ll * 365ll);

    FTruongSinhActionCommand RepeatedBreakthrough = Plan.Action;
    RepeatedBreakthrough.CommandId = FGuid(75, 76, 77, 78);
    RepeatedBreakthrough.ExpectedWorldRevision = State.WorldRevision;
    RepeatedBreakthrough.Sequence = State.CommittedCommandIds.Num();
    const FTruongSinhActionResult Repeated = FTruongSinhGameSimulation::Execute(State, RepeatedBreakthrough);
    TestEqual(TEXT("A later command cannot farm the same realm breakthrough"),
        Repeated.Status, ETruongSinhActionStatus::Rejected);
    TestEqual(TEXT("Repeated realm transition reports its canonical precondition"),
        Repeated.ReasonId.Value, FString(TEXT("activity.reject.precondition")));
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FTruongSinhAlchemyOutputSpec,
    "TruongSinh.Activity.AlchemyOutputAndSave",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FTruongSinhAlchemyOutputSpec::RunTest(const FString& Parameters)
{
    FTruongSinhSimulationState State = FTruongSinhGameSimulation::CreateNewGame(141210);
    FTruongSinhActivityPlan Plan;
    Plan.Action.CommandId = FGuid(81, 82, 83, 84);
    Plan.Action.ActionId.Value = FTruongSinhGameSimulation::CommitResolvedActivityActionId;
    Plan.Action.InstigatorId = State.CurrentVessel.VesselId;
    Plan.Action.ExpectedWorldRevision = State.WorldRevision;
    Plan.Type = ETruongSinhActivityType::Alchemy;
    Plan.ActivityId.Value = TEXT("activity.alchemy.qingxin_pill");
    Plan.MethodId.Value = TEXT("method.five_elements_breathing");
    Plan.FacilityId.Value = TEXT("facility.alchemy.qingxin");
    Plan.LocationId.Value = TEXT("zone.lower_realm.dev_smoke");
    Plan.DurationMinutes = 360;
    Plan.OutputId.Value = TEXT("pill.qingxin");
    Plan.MaximumOutputUnits = 3;

    FTruongSinhActivitySnapshot Snapshot;
    Snapshot.PerformerPower = 10000;
    Snapshot.DifficultyOrTargetPower = 6500;
    Snapshot.TechniqueModifierUnits = 400;
    Snapshot.PreparationModifierUnits = 350;
    Snapshot.EnvironmentModifierUnits = 300;
    Snapshot.MasterSeed = State.Rng.MasterSeed;
    const FTruongSinhAutoResolutionResult Resolution = FTruongSinhAutoResolver::Resolve(Snapshot, Plan);
    TestEqual(TEXT("Alchemy resolve is a deterministic great success at the configured power gap"),
        Resolution.Outcome, ETruongSinhResolutionOutcome::GreatSuccess);
    TestEqual(TEXT("Alchemy returns the authored output ID"), Resolution.OutputId.Value, FString(TEXT("pill.qingxin")));
    TestEqual(TEXT("Great alchemy returns the recipe maximum"), Resolution.OutputUnits, 3ll);
    TestEqual(TEXT("Great alchemy quality is deterministic"), Resolution.OutputQualityBps, 9500);
    TestEqual(TEXT("Great alchemy impurity is deterministic"), Resolution.OutputImpurityBps, 500);

    FTruongSinhResolvedActivityCommitPayload Payload;
    Payload.ActivityId = Plan.ActivityId;
    Payload.RequiredCurrentRealmId = State.CurrentVessel.RealmId;
    Payload.Minutes = Resolution.TimeAdvancedMinutes;
    Payload.OutcomeId = Resolution.OutcomeId;
    Payload.ReplayId = Resolution.ReplayId;
    Payload.OutputId = Resolution.OutputId;
    Payload.OutputUnits = Resolution.OutputUnits;
    Payload.OutputQualityBps = Resolution.OutputQualityBps;
    Payload.OutputImpurityBps = Resolution.OutputImpurityBps;
    Plan.Action.Payload.InitializeAs<FTruongSinhResolvedActivityCommitPayload>(Payload);
    const FTruongSinhActionResult Commit = FTruongSinhGameSimulation::Execute(State, Plan.Action);
    TestEqual(TEXT("Alchemy output commits through the shared gateway"), Commit.Status, ETruongSinhActionStatus::Committed);
    TestEqual(TEXT("One canonical output record is retained"), State.ActivityOutputRecords.Num(), 1);
    if (State.ActivityOutputRecords.Num() == 1)
    {
        TestEqual(TEXT("Ledger stores exact output quantity"), State.ActivityOutputRecords[0].Units, 3ll);
        TestEqual(TEXT("Ledger stores quality"), State.ActivityOutputRecords[0].QualityBps, 9500);
        TestEqual(TEXT("Ledger stores impurity"), State.ActivityOutputRecords[0].ImpurityBps, 500);
    }

    FTruongSinhSaveGameV2 Save;
    Save.Simulation = State;
    Save.PayloadHash = FTruongSinhGameSimulation::ComputeStateHash(State);
    FString Json;
    FString Error;
    TestTrue(TEXT("Alchemy output save serializes"), FTruongSinhSaveJsonCodecV2::Serialize(Save, Json, Error));
    FTruongSinhSaveGameV2 Loaded;
    TestTrue(TEXT("Alchemy output save restores"), FTruongSinhSaveJsonCodecV2::Deserialize(Json, Loaded, Error));
    TestEqual(TEXT("Save preserves the alchemy output ledger"), Loaded.Simulation.ActivityOutputRecords.Num(), 1);
    if (Loaded.Simulation.ActivityOutputRecords.Num() == 1)
    {
        TestEqual(TEXT("Save preserves output identity"),
            Loaded.Simulation.ActivityOutputRecords[0].OutputId.Value, FString(TEXT("pill.qingxin")));
    }
    return true;
}

#endif
