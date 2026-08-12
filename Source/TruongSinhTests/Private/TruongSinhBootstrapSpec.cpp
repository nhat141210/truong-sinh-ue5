#if WITH_DEV_AUTOMATION_TESTS

#include "Core/TruongSinhTypes.h"
#include "Core/TruongSinhDeterministicRng.h"
#include "Misc/AutomationTest.h"
#include "Save/TruongSinhSaveGameV1.h"
#include "Simulation/TruongSinhGameSimulation.h"

namespace
{
FTruongSinhActionCommand MakeAdvanceTimeCommand(
    const FGuid CommandId,
    const int64 ExpectedRevision,
    const int32 Days)
{
    FTruongSinhActionCommand Command;
    Command.CommandId = CommandId;
    Command.ActionId.Value = FTruongSinhGameSimulation::AdvanceTimeActionId;
    Command.InstigatorId.Value = TEXT("player.main");
    Command.ExpectedWorldRevision = ExpectedRevision;
    Command.Sequence = ExpectedRevision;

    FTruongSinhAdvanceTimePayload Payload;
    Payload.DayCount = Days;
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
    FTruongSinhRngStreamState CombatStream =
        FTruongSinhDeterministicRng::CreateStream(42, TEXT("combat.turn"));

    TestNotEqual(
        TEXT("Stable stream IDs derive independent sequences"),
        FTruongSinhDeterministicRng::NextUInt64(WorldStream),
        FTruongSinhDeterministicRng::NextUInt64(CombatStream));

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
    const FTruongSinhActionCommand Command = MakeAdvanceTimeCommand(CommandId, 0, 7);

    const FTruongSinhActionResult First = FTruongSinhGameSimulation::Execute(State, Command);
    TestEqual(TEXT("Valid action commits"), First.Status, ETruongSinhActionStatus::Committed);
    TestEqual(TEXT("Time advances only by command payload"), State.ElapsedDays, 7ll);
    TestEqual(TEXT("Revision advances once"), State.WorldRevision, 1ll);
    TestEqual(TEXT("One ordered event is emitted"), First.Events.Num(), 1);
    TestFalse(TEXT("Committed state has a hash"), First.StateHash.IsEmpty());

    const FString HashAfterFirst = FTruongSinhGameSimulation::ComputeStateHash(State);
    const FTruongSinhActionResult Duplicate = FTruongSinhGameSimulation::Execute(State, Command);
    TestEqual(TEXT("Duplicate command is rejected"), Duplicate.Status, ETruongSinhActionStatus::Rejected);
    TestEqual(TEXT("Duplicate command cannot charge time twice"), State.ElapsedDays, 7ll);
    TestEqual(TEXT("Duplicate command cannot advance revision"), State.WorldRevision, 1ll);
    TestEqual(TEXT("Rejected command leaves hash unchanged"), FTruongSinhGameSimulation::ComputeStateHash(State), HashAfterFirst);

    const FTruongSinhActionCommand Stale = MakeAdvanceTimeCommand(FGuid(5, 6, 7, 8), 0, 2);
    const FTruongSinhActionResult StaleResult = FTruongSinhGameSimulation::Execute(State, Stale);
    TestEqual(TEXT("Stale revision is rejected"), StaleResult.Status, ETruongSinhActionStatus::Rejected);
    TestEqual(TEXT("Stale command leaves time unchanged"), State.ElapsedDays, 7ll);
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
    const FTruongSinhActionCommand Command = MakeAdvanceTimeCommand(FGuid(11, 12, 13, 14), 0, 30);

    const FTruongSinhActionResult LeftResult = FTruongSinhGameSimulation::Execute(Left, Command);
    const FTruongSinhActionResult RightResult = FTruongSinhGameSimulation::Execute(Right, Command);
    TestEqual(TEXT("Same seed and command produce same result hash"), LeftResult.StateHash, RightResult.StateHash);
    TestEqual(TEXT("Same input produces same elapsed days"), Left.ElapsedDays, Right.ElapsedDays);
    TestEqual(TEXT("Same input produces same revision"), Left.WorldRevision, Right.WorldRevision);
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FTruongSinhSaveRoundTripSpec,
    "TruongSinh.Save.RoundTripV1",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FTruongSinhSaveRoundTripSpec::RunTest(const FString& Parameters)
{
    FTruongSinhSaveGameV1 Save;
    Save.GameBuildId = TEXT("automation-test");
    Save.Simulation = FTruongSinhGameSimulation::CreateNewGame(987654321);

    FTruongSinhRngStreamState Stream =
        FTruongSinhDeterministicRng::CreateStream(Save.Simulation.Rng.MasterSeed, TEXT("world.events"));
    FTruongSinhDeterministicRng::NextUInt64(Stream);
    Save.Simulation.Rng.Streams.Add(Stream);

    const FTruongSinhActionCommand Command = MakeAdvanceTimeCommand(FGuid(21, 22, 23, 24), 0, 9);
    FTruongSinhGameSimulation::Execute(Save.Simulation, Command);
    Save.PayloadHash = FTruongSinhGameSimulation::ComputeStateHash(Save.Simulation);

    FString Json;
    FString Error;
    TestTrue(TEXT("Save v1 serializes"), FTruongSinhSaveJsonCodec::Serialize(Save, Json, Error));
    if (!Error.IsEmpty())
    {
        AddError(Error);
    }

    FTruongSinhSaveGameV1 Loaded;
    const bool bLoaded = FTruongSinhSaveJsonCodec::Deserialize(Json, Loaded, Error);
    TestTrue(TEXT("Save v1 deserializes"), bLoaded);
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

    FString CorruptJson = Json;
    const FString HashNeedle = FString::Printf(TEXT("\"%s\""), *Save.PayloadHash);
    TestTrue(TEXT("Serialized payload contains its canonical hash"), CorruptJson.Contains(HashNeedle));
    CorruptJson = CorruptJson.Replace(
        *HashNeedle,
        TEXT("\"blake3-v1:0000000000000000000000000000000000000000000000000000000000000000\""));
    FTruongSinhSaveGameV1 Corrupt;
    TestFalse(TEXT("Payload tampering fails checksum"),
        FTruongSinhSaveJsonCodec::Deserialize(CorruptJson, Corrupt, Error));
    return true;
}

#endif
