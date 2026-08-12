#include "Save/TruongSinhSaveGameV2.h"

#include "Containers/UnrealString.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"
#include "Serialization/JsonWriter.h"

namespace
{
template <typename IntegerType>
void SetInteger(const TSharedRef<FJsonObject>& Object, const TCHAR* Field, const IntegerType Value)
{
    Object->SetStringField(Field, LexToString(Value));
}

template <typename IntegerType>
bool GetInteger(
    const TSharedPtr<FJsonObject>& Object,
    const TCHAR* Field,
    IntegerType& OutValue,
    FString& OutError)
{
    FString Text;
    if (!Object.IsValid() || !Object->TryGetStringField(Field, Text) ||
        !LexTryParseString(OutValue, *Text))
    {
        OutError = FString::Printf(TEXT("Missing or invalid integer string: %s"), Field);
        return false;
    }
    return true;
}

TArray<TSharedPtr<FJsonValue>> StableIdArray(const TArray<FTruongSinhStableId>& Ids)
{
    TArray<FString> Sorted;
    for (const FTruongSinhStableId& Id : Ids)
    {
        Sorted.Add(Id.Value);
    }
    Sorted.Sort();

    TArray<TSharedPtr<FJsonValue>> Values;
    for (const FString& Value : Sorted)
    {
        Values.Add(MakeShared<FJsonValueString>(Value));
    }
    return Values;
}

bool GetStableIdArray(
    const TSharedPtr<FJsonObject>& Object,
    const TCHAR* Field,
    TArray<FTruongSinhStableId>& OutIds,
    FString& OutError)
{
    const TArray<TSharedPtr<FJsonValue>>* Values = nullptr;
    if (!Object.IsValid() || !Object->TryGetArrayField(Field, Values) || !Values)
    {
        OutError = FString::Printf(TEXT("Missing stable ID array: %s"), Field);
        return false;
    }

    TSet<FString> Unique;
    for (const TSharedPtr<FJsonValue>& Value : *Values)
    {
        FString Text;
        FTruongSinhStableId Id;
        if (!Value.IsValid() || !Value->TryGetString(Text))
        {
            OutError = FString::Printf(TEXT("Invalid stable ID array entry: %s"), Field);
            return false;
        }
        Id.Value = Text;
        if (!Id.IsValid() || Unique.Contains(Text))
        {
            OutError = FString::Printf(TEXT("Invalid or duplicate stable ID: %s"), *Text);
            return false;
        }
        Unique.Add(Text);
        OutIds.Add(MoveTemp(Id));
    }
    return true;
}

bool GetStableId(
    const TSharedPtr<FJsonObject>& Object,
    const TCHAR* Field,
    FTruongSinhStableId& OutId,
    const bool bAllowEmpty,
    FString& OutError)
{
    if (!Object.IsValid() || !Object->TryGetStringField(Field, OutId.Value) ||
        (!bAllowEmpty && !OutId.IsValid()) ||
        (bAllowEmpty && !OutId.Value.IsEmpty() && !OutId.IsValid()))
    {
        OutError = FString::Printf(TEXT("Invalid stable ID: %s"), Field);
        return false;
    }
    return true;
}
}

bool FTruongSinhSaveJsonCodecV2::Serialize(
    const FTruongSinhSaveGameV2& Save,
    FString& OutJson,
    FString& OutError)
{
    OutJson.Reset();
    OutError.Reset();
    if (Save.SaveSchemaVersion != FTruongSinhSaveGameV2::CurrentSchemaVersion)
    {
        OutError = TEXT("Only save schema v2 is supported.");
        return false;
    }

    const FString Hash = FTruongSinhGameSimulation::ComputeStateHash(Save.Simulation);
    if (!Save.PayloadHash.IsEmpty() && Save.PayloadHash != Hash)
    {
        OutError = TEXT("PayloadHash does not match canonical state.");
        return false;
    }

    const TSharedRef<FJsonObject> Root = MakeShared<FJsonObject>();
    SetInteger(Root, TEXT("schema"), Save.SaveSchemaVersion);
    Root->SetStringField(TEXT("build"), Save.GameBuildId);
    Root->SetStringField(TEXT("payload_hash"), Hash);
    Root->SetStringField(TEXT("pending_replay"), Save.PendingReplayId.Value);

    const FTruongSinhSimulationState& State = Save.Simulation;
    const TSharedRef<FJsonObject> Simulation = MakeShared<FJsonObject>();
    SetInteger(Simulation, TEXT("state_schema"), State.SchemaVersion);
    SetInteger(Simulation, TEXT("elapsed_minutes"), State.ElapsedMinutes);
    SetInteger(Simulation, TEXT("exploration_remainder_ms"), State.ExplorationRemainderMillis);
    SetInteger(Simulation, TEXT("world_revision"), State.WorldRevision);
    Simulation->SetStringField(TEXT("world_layer"), State.WorldLayerId.Value);

    const TSharedRef<FJsonObject> Soul = MakeShared<FJsonObject>();
    Soul->SetStringField(TEXT("id"), State.Soul.SoulId.Value);
    SetInteger(Soul, TEXT("integrity"), State.Soul.IntegrityUnits);
    Soul->SetArrayField(TEXT("known_techniques"), StableIdArray(State.Soul.KnownTechniqueIds));
    Soul->SetArrayField(TEXT("memories"), StableIdArray(State.Soul.MemoryIds));
    Soul->SetArrayField(TEXT("vessel_history"), StableIdArray(State.Soul.VesselHistoryIds));
    Simulation->SetObjectField(TEXT("soul"), Soul);

    const FTruongSinhVesselState& VesselState = State.CurrentVessel;
    const TSharedRef<FJsonObject> Vessel = MakeShared<FJsonObject>();
    Vessel->SetStringField(TEXT("id"), VesselState.VesselId.Value);
    Vessel->SetStringField(TEXT("identity"), VesselState.IdentityId.Value);
    Vessel->SetStringField(TEXT("spiritual_root"), VesselState.SpiritualRootId.Value);
    Vessel->SetStringField(TEXT("realm"), VesselState.RealmId.Value);
    Vessel->SetStringField(TEXT("sect"), VesselState.SectId.Value);
    SetInteger(Vessel, TEXT("cultivation"), VesselState.CultivationUnits);
    Vessel->SetArrayField(TEXT("relationships"), StableIdArray(VesselState.RelationshipIds));
    Vessel->SetArrayField(TEXT("owned_assets"), StableIdArray(VesselState.OwnedAssetIds));

    const TSharedRef<FJsonObject> Lifespan = MakeShared<FJsonObject>();
    SetInteger(Lifespan, TEXT("biological_age_days"), VesselState.Lifespan.BiologicalAgeDays);
    SetInteger(Lifespan, TEXT("base_days"), VesselState.Lifespan.BaseLifespanDays);
    SetInteger(Lifespan, TEXT("realm_bonus_days"), VesselState.Lifespan.RealmBonusDays);
    SetInteger(Lifespan, TEXT("technique_bonus_days"), VesselState.Lifespan.TechniqueBonusDays);
    SetInteger(Lifespan, TEXT("pill_resource_bonus_days"), VesselState.Lifespan.PillAndResourceBonusDays);
    SetInteger(Lifespan, TEXT("permanent_damage_days"), VesselState.Lifespan.PermanentDamageDays);
    Vessel->SetObjectField(TEXT("lifespan"), Lifespan);
    Simulation->SetObjectField(TEXT("vessel"), Vessel);

    const TSharedRef<FJsonObject> Rng = MakeShared<FJsonObject>();
    SetInteger(Rng, TEXT("algorithm"), State.Rng.AlgorithmVersion);
    SetInteger(Rng, TEXT("master_seed"), State.Rng.MasterSeed);
    TArray<FTruongSinhRngStreamState> Streams = State.Rng.Streams;
    Streams.Sort([](const FTruongSinhRngStreamState& A, const FTruongSinhRngStreamState& B)
    {
        return A.StreamId < B.StreamId;
    });
    TArray<TSharedPtr<FJsonValue>> StreamValues;
    for (const FTruongSinhRngStreamState& Stream : Streams)
    {
        const TSharedRef<FJsonObject> Item = MakeShared<FJsonObject>();
        Item->SetStringField(TEXT("id"), Stream.StreamId);
        Item->SetStringField(TEXT("state_hex"), FString::Printf(TEXT("%016llx"),
            static_cast<unsigned long long>(Stream.State)));
        SetInteger(Item, TEXT("draw_count"), Stream.DrawCount);
        StreamValues.Add(MakeShared<FJsonValueObject>(Item));
    }
    Rng->SetArrayField(TEXT("streams"), StreamValues);
    Simulation->SetObjectField(TEXT("rng"), Rng);

    TArray<FString> Commands;
    for (const FGuid& Id : State.CommittedCommandIds)
    {
        Commands.Add(Id.ToString(EGuidFormats::Digits));
    }
    Commands.Sort();
    TArray<TSharedPtr<FJsonValue>> CommandValues;
    for (const FString& Id : Commands)
    {
        CommandValues.Add(MakeShared<FJsonValueString>(Id));
    }
    Simulation->SetArrayField(TEXT("committed_commands"), CommandValues);
    Root->SetObjectField(TEXT("simulation"), Simulation);

    const TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutJson);
    if (!FJsonSerializer::Serialize(Root, Writer))
    {
        OutError = TEXT("JSON serialization failed.");
        return false;
    }
    return true;
}

bool FTruongSinhSaveJsonCodecV2::Deserialize(
    const FString& Json,
    FTruongSinhSaveGameV2& OutSave,
    FString& OutError)
{
    OutError.Reset();
    TSharedPtr<FJsonObject> Root;
    const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Json);
    if (!FJsonSerializer::Deserialize(Reader, Root) || !Root.IsValid())
    {
        OutError = TEXT("Invalid JSON document.");
        return false;
    }

    FTruongSinhSaveGameV2 Candidate;
    if (!GetInteger(Root, TEXT("schema"), Candidate.SaveSchemaVersion, OutError) ||
        Candidate.SaveSchemaVersion != FTruongSinhSaveGameV2::CurrentSchemaVersion)
    {
        OutError = TEXT("Unsupported save schema; v1 development migration is not yet implemented.");
        return false;
    }
    if (!Root->TryGetStringField(TEXT("build"), Candidate.GameBuildId) ||
        !Root->TryGetStringField(TEXT("payload_hash"), Candidate.PayloadHash) ||
        !Root->TryGetStringField(TEXT("pending_replay"), Candidate.PendingReplayId.Value))
    {
        OutError = TEXT("Missing save header.");
        return false;
    }
    if (!Candidate.PendingReplayId.Value.IsEmpty() && !Candidate.PendingReplayId.IsValid())
    {
        OutError = TEXT("Invalid pending replay ID.");
        return false;
    }

    const TSharedPtr<FJsonObject>* SimulationPtr = nullptr;
    if (!Root->TryGetObjectField(TEXT("simulation"), SimulationPtr) || !SimulationPtr)
    {
        OutError = TEXT("Missing simulation object.");
        return false;
    }
    const TSharedPtr<FJsonObject> Simulation = *SimulationPtr;
    FTruongSinhSimulationState& State = Candidate.Simulation;
    if (!GetInteger(Simulation, TEXT("state_schema"), State.SchemaVersion, OutError) ||
        !GetInteger(Simulation, TEXT("elapsed_minutes"), State.ElapsedMinutes, OutError) ||
        !GetInteger(Simulation, TEXT("exploration_remainder_ms"), State.ExplorationRemainderMillis, OutError) ||
        !GetInteger(Simulation, TEXT("world_revision"), State.WorldRevision, OutError) ||
        !GetStableId(Simulation, TEXT("world_layer"), State.WorldLayerId, false, OutError))
    {
        return false;
    }

    const TSharedPtr<FJsonObject>* SoulPtr = nullptr;
    if (!Simulation->TryGetObjectField(TEXT("soul"), SoulPtr) || !SoulPtr)
    {
        OutError = TEXT("Missing soul object.");
        return false;
    }
    const TSharedPtr<FJsonObject> Soul = *SoulPtr;
    if (!GetStableId(Soul, TEXT("id"), State.Soul.SoulId, false, OutError) ||
        !GetInteger(Soul, TEXT("integrity"), State.Soul.IntegrityUnits, OutError) ||
        !GetStableIdArray(Soul, TEXT("known_techniques"), State.Soul.KnownTechniqueIds, OutError) ||
        !GetStableIdArray(Soul, TEXT("memories"), State.Soul.MemoryIds, OutError) ||
        !GetStableIdArray(Soul, TEXT("vessel_history"), State.Soul.VesselHistoryIds, OutError))
    {
        return false;
    }

    const TSharedPtr<FJsonObject>* VesselPtr = nullptr;
    if (!Simulation->TryGetObjectField(TEXT("vessel"), VesselPtr) || !VesselPtr)
    {
        OutError = TEXT("Missing vessel object.");
        return false;
    }
    const TSharedPtr<FJsonObject> Vessel = *VesselPtr;
    FTruongSinhVesselState& VesselState = State.CurrentVessel;
    if (!GetStableId(Vessel, TEXT("id"), VesselState.VesselId, false, OutError) ||
        !GetStableId(Vessel, TEXT("identity"), VesselState.IdentityId, false, OutError) ||
        !GetStableId(Vessel, TEXT("spiritual_root"), VesselState.SpiritualRootId, false, OutError) ||
        !GetStableId(Vessel, TEXT("realm"), VesselState.RealmId, false, OutError) ||
        !GetStableId(Vessel, TEXT("sect"), VesselState.SectId, true, OutError) ||
        !GetInteger(Vessel, TEXT("cultivation"), VesselState.CultivationUnits, OutError) ||
        !GetStableIdArray(Vessel, TEXT("relationships"), VesselState.RelationshipIds, OutError) ||
        !GetStableIdArray(Vessel, TEXT("owned_assets"), VesselState.OwnedAssetIds, OutError))
    {
        return false;
    }

    const TSharedPtr<FJsonObject>* LifespanPtr = nullptr;
    if (!Vessel->TryGetObjectField(TEXT("lifespan"), LifespanPtr) || !LifespanPtr)
    {
        OutError = TEXT("Missing lifespan object.");
        return false;
    }
    const TSharedPtr<FJsonObject> Lifespan = *LifespanPtr;
    if (!GetInteger(Lifespan, TEXT("biological_age_days"), VesselState.Lifespan.BiologicalAgeDays, OutError) ||
        !GetInteger(Lifespan, TEXT("base_days"), VesselState.Lifespan.BaseLifespanDays, OutError) ||
        !GetInteger(Lifespan, TEXT("realm_bonus_days"), VesselState.Lifespan.RealmBonusDays, OutError) ||
        !GetInteger(Lifespan, TEXT("technique_bonus_days"), VesselState.Lifespan.TechniqueBonusDays, OutError) ||
        !GetInteger(Lifespan, TEXT("pill_resource_bonus_days"), VesselState.Lifespan.PillAndResourceBonusDays, OutError) ||
        !GetInteger(Lifespan, TEXT("permanent_damage_days"), VesselState.Lifespan.PermanentDamageDays, OutError))
    {
        return false;
    }

    const TSharedPtr<FJsonObject>* RngPtr = nullptr;
    if (!Simulation->TryGetObjectField(TEXT("rng"), RngPtr) || !RngPtr)
    {
        OutError = TEXT("Missing RNG object.");
        return false;
    }
    const TSharedPtr<FJsonObject> Rng = *RngPtr;
    if (!GetInteger(Rng, TEXT("algorithm"), State.Rng.AlgorithmVersion, OutError) ||
        !GetInteger(Rng, TEXT("master_seed"), State.Rng.MasterSeed, OutError))
    {
        return false;
    }
    const TArray<TSharedPtr<FJsonValue>>* StreamValues = nullptr;
    if (!Rng->TryGetArrayField(TEXT("streams"), StreamValues) || !StreamValues)
    {
        OutError = TEXT("Missing RNG streams.");
        return false;
    }
    TSet<FString> StreamIds;
    for (const TSharedPtr<FJsonValue>& Value : *StreamValues)
    {
        const TSharedPtr<FJsonObject> Item = Value.IsValid() ? Value->AsObject() : nullptr;
        FTruongSinhRngStreamState Stream;
        FString StateHex;
        if (!Item.IsValid() || !Item->TryGetStringField(TEXT("id"), Stream.StreamId) ||
            !Item->TryGetStringField(TEXT("state_hex"), StateHex) ||
            !GetInteger(Item, TEXT("draw_count"), Stream.DrawCount, OutError))
        {
            OutError = TEXT("Invalid RNG stream.");
            return false;
        }
        FTruongSinhStableId StreamId;
        StreamId.Value = Stream.StreamId;
        TCHAR* End = nullptr;
        Stream.State = FCString::Strtoui64(*StateHex, &End, 16);
        if (!StreamId.IsValid() || StreamIds.Contains(Stream.StreamId) ||
            Stream.DrawCount < 0 || StateHex.Len() != 16 || !End || *End != TEXT('\0'))
        {
            OutError = TEXT("Invalid or duplicate RNG stream state.");
            return false;
        }
        StreamIds.Add(Stream.StreamId);
        State.Rng.Streams.Add(MoveTemp(Stream));
    }

    const TArray<TSharedPtr<FJsonValue>>* CommandValues = nullptr;
    if (!Simulation->TryGetArrayField(TEXT("committed_commands"), CommandValues) || !CommandValues)
    {
        OutError = TEXT("Missing committed commands.");
        return false;
    }
    TSet<FGuid> CommandIds;
    for (const TSharedPtr<FJsonValue>& Value : *CommandValues)
    {
        FString Text;
        FGuid Id;
        if (!Value.IsValid() || !Value->TryGetString(Text) ||
            !FGuid::ParseExact(Text, EGuidFormats::Digits, Id) || CommandIds.Contains(Id))
        {
            OutError = TEXT("Invalid or duplicate command ID.");
            return false;
        }
        CommandIds.Add(Id);
        State.CommittedCommandIds.Add(Id);
    }

    if (State.SchemaVersion != 2 || State.ElapsedMinutes < 0 ||
        State.ExplorationRemainderMillis < 0 || State.WorldRevision < 0 ||
        State.Rng.AlgorithmVersion != 1 || State.Soul.IntegrityUnits < 0 ||
        State.Soul.IntegrityUnits > 10000 || VesselState.CultivationUnits < 0 ||
        VesselState.Lifespan.BiologicalAgeDays < 0 || VesselState.Lifespan.BaseLifespanDays < 0 ||
        VesselState.Lifespan.RealmBonusDays < 0 || VesselState.Lifespan.TechniqueBonusDays < 0 ||
        VesselState.Lifespan.PillAndResourceBonusDays < 0 ||
        VesselState.Lifespan.PermanentDamageDays < 0)
    {
        OutError = TEXT("Invalid sandbox state values.");
        return false;
    }
    if (FTruongSinhGameSimulation::ComputeStateHash(State) != Candidate.PayloadHash)
    {
        OutError = TEXT("Payload hash mismatch.");
        return false;
    }

    OutSave = MoveTemp(Candidate);
    return true;
}
