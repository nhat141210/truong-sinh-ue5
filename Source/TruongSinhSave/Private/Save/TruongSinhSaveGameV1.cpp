#include "Save/TruongSinhSaveGameV1.h"

#include "Dom/JsonObject.h"
#include "Containers/UnrealString.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"
#include "Serialization/JsonWriter.h"

namespace TruongSinhSaveFields
{
constexpr TCHAR Schema[] = TEXT("schema");
constexpr TCHAR Build[] = TEXT("build");
constexpr TCHAR PayloadHash[] = TEXT("payload_hash");
constexpr TCHAR StateSchema[] = TEXT("state_schema");
constexpr TCHAR ElapsedDays[] = TEXT("elapsed_days");
constexpr TCHAR Revision[] = TEXT("world_revision");
constexpr TCHAR RngAlgorithm[] = TEXT("rng_algorithm");
constexpr TCHAR MasterSeed[] = TEXT("master_seed");
constexpr TCHAR Streams[] = TEXT("rng_streams");
constexpr TCHAR StreamId[] = TEXT("id");
constexpr TCHAR StreamState[] = TEXT("state_hex");
constexpr TCHAR DrawCount[] = TEXT("draw_count");
constexpr TCHAR Commands[] = TEXT("committed_commands");
}

namespace
{
template <typename IntegerType>
void SetIntegerString(const TSharedRef<FJsonObject>& Object, const TCHAR* Field, const IntegerType Value)
{
    Object->SetStringField(Field, LexToString(Value));
}

template <typename IntegerType>
bool TryGetIntegerString(
    const TSharedPtr<FJsonObject>& Object,
    const TCHAR* Field,
    IntegerType& OutValue,
    FString& OutError)
{
    FString Text;
    if (!Object.IsValid() || !Object->TryGetStringField(Field, Text) || !LexTryParseString(OutValue, *Text))
    {
        OutError = FString::Printf(TEXT("Missing or invalid integer string: %s"), Field);
        return false;
    }
    return true;
}
}

bool FTruongSinhSaveJsonCodec::Serialize(
    const FTruongSinhSaveGameV1& Save,
    FString& OutJson,
    FString& OutError)
{
    OutJson.Reset();
    OutError.Reset();

    if (Save.SaveSchemaVersion != FTruongSinhSaveGameV1::CurrentSchemaVersion)
    {
        OutError = TEXT("Only save schema v1 can be serialized by the v1 codec.");
        return false;
    }

    const FString CanonicalHash = FTruongSinhGameSimulation::ComputeStateHash(Save.Simulation);
    if (!Save.PayloadHash.IsEmpty() && Save.PayloadHash != CanonicalHash)
    {
        OutError = TEXT("PayloadHash does not match the canonical simulation state.");
        return false;
    }

    const TSharedRef<FJsonObject> Root = MakeShared<FJsonObject>();
    SetIntegerString(Root, TruongSinhSaveFields::Schema, Save.SaveSchemaVersion);
    Root->SetStringField(TruongSinhSaveFields::Build, Save.GameBuildId);
    Root->SetStringField(TruongSinhSaveFields::PayloadHash, CanonicalHash);
    SetIntegerString(Root, TruongSinhSaveFields::StateSchema, Save.Simulation.SchemaVersion);
    SetIntegerString(Root, TruongSinhSaveFields::ElapsedDays, Save.Simulation.ElapsedDays);
    SetIntegerString(Root, TruongSinhSaveFields::Revision, Save.Simulation.WorldRevision);
    SetIntegerString(Root, TruongSinhSaveFields::RngAlgorithm, Save.Simulation.Rng.AlgorithmVersion);
    SetIntegerString(Root, TruongSinhSaveFields::MasterSeed, Save.Simulation.Rng.MasterSeed);

    TArray<FTruongSinhRngStreamState> Streams = Save.Simulation.Rng.Streams;
    Streams.Sort([](const FTruongSinhRngStreamState& Left, const FTruongSinhRngStreamState& Right)
    {
        return Left.StreamId < Right.StreamId;
    });

    TArray<TSharedPtr<FJsonValue>> StreamValues;
    for (const FTruongSinhRngStreamState& Stream : Streams)
    {
        const TSharedRef<FJsonObject> StreamObject = MakeShared<FJsonObject>();
        StreamObject->SetStringField(TruongSinhSaveFields::StreamId, Stream.StreamId);
        StreamObject->SetStringField(
            TruongSinhSaveFields::StreamState,
            FString::Printf(TEXT("%016llx"), static_cast<unsigned long long>(Stream.State)));
        SetIntegerString(StreamObject, TruongSinhSaveFields::DrawCount, Stream.DrawCount);
        StreamValues.Add(MakeShared<FJsonValueObject>(StreamObject));
    }
    Root->SetArrayField(TruongSinhSaveFields::Streams, StreamValues);

    TArray<FString> Commands;
    for (const FGuid& CommandId : Save.Simulation.CommittedCommandIds)
    {
        Commands.Add(CommandId.ToString(EGuidFormats::Digits));
    }
    Commands.Sort();
    TArray<TSharedPtr<FJsonValue>> CommandValues;
    for (const FString& Command : Commands)
    {
        CommandValues.Add(MakeShared<FJsonValueString>(Command));
    }
    Root->SetArrayField(TruongSinhSaveFields::Commands, CommandValues);

    const TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutJson);
    if (!FJsonSerializer::Serialize(Root, Writer))
    {
        OutError = TEXT("JSON serialization failed.");
        return false;
    }
    return true;
}

bool FTruongSinhSaveJsonCodec::Deserialize(
    const FString& Json,
    FTruongSinhSaveGameV1& OutSave,
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

    FTruongSinhSaveGameV1 Candidate;
    if (!TryGetIntegerString(Root, TruongSinhSaveFields::Schema, Candidate.SaveSchemaVersion, OutError) ||
        Candidate.SaveSchemaVersion != FTruongSinhSaveGameV1::CurrentSchemaVersion)
    {
        if (OutError.IsEmpty())
        {
            OutError = TEXT("Unsupported save schema.");
        }
        return false;
    }
    if (!Root->TryGetStringField(TruongSinhSaveFields::Build, Candidate.GameBuildId) ||
        !Root->TryGetStringField(TruongSinhSaveFields::PayloadHash, Candidate.PayloadHash) ||
        !TryGetIntegerString(Root, TruongSinhSaveFields::StateSchema, Candidate.Simulation.SchemaVersion, OutError) ||
        !TryGetIntegerString(Root, TruongSinhSaveFields::ElapsedDays, Candidate.Simulation.ElapsedDays, OutError) ||
        !TryGetIntegerString(Root, TruongSinhSaveFields::Revision, Candidate.Simulation.WorldRevision, OutError) ||
        !TryGetIntegerString(Root, TruongSinhSaveFields::RngAlgorithm, Candidate.Simulation.Rng.AlgorithmVersion, OutError) ||
        !TryGetIntegerString(Root, TruongSinhSaveFields::MasterSeed, Candidate.Simulation.Rng.MasterSeed, OutError))
    {
        if (OutError.IsEmpty())
        {
            OutError = TEXT("Save header or simulation fields are missing.");
        }
        return false;
    }

    const TArray<TSharedPtr<FJsonValue>>* StreamValues = nullptr;
    if (!Root->TryGetArrayField(TruongSinhSaveFields::Streams, StreamValues) || !StreamValues)
    {
        OutError = TEXT("Missing RNG streams.");
        return false;
    }
    for (const TSharedPtr<FJsonValue>& StreamValue : *StreamValues)
    {
        const TSharedPtr<FJsonObject> StreamObject = StreamValue.IsValid() ? StreamValue->AsObject() : nullptr;
        FTruongSinhRngStreamState Stream;
        FString StateHex;
        if (!StreamObject.IsValid() ||
            !StreamObject->TryGetStringField(TruongSinhSaveFields::StreamId, Stream.StreamId) ||
            !StreamObject->TryGetStringField(TruongSinhSaveFields::StreamState, StateHex) ||
            !TryGetIntegerString(StreamObject, TruongSinhSaveFields::DrawCount, Stream.DrawCount, OutError))
        {
            OutError = TEXT("Invalid RNG stream entry.");
            return false;
        }
        TCHAR* End = nullptr;
        Stream.State = FCString::Strtoui64(*StateHex, &End, 16);
        if (!End || *End != TEXT('\0') || StateHex.Len() != 16)
        {
            OutError = TEXT("Invalid RNG state hex.");
            return false;
        }
        FTruongSinhStableId StableStreamId;
        StableStreamId.Value = Stream.StreamId;
        if (!StableStreamId.IsValid() || Stream.DrawCount < 0)
        {
            OutError = TEXT("Invalid canonical RNG stream state.");
            return false;
        }
        Candidate.Simulation.Rng.Streams.Add(MoveTemp(Stream));
    }

    const TArray<TSharedPtr<FJsonValue>>* CommandValues = nullptr;
    if (!Root->TryGetArrayField(TruongSinhSaveFields::Commands, CommandValues) || !CommandValues)
    {
        OutError = TEXT("Missing committed command list.");
        return false;
    }
    for (const TSharedPtr<FJsonValue>& CommandValue : *CommandValues)
    {
        FString CommandText;
        FGuid CommandId;
        if (!CommandValue.IsValid() || !CommandValue->TryGetString(CommandText) ||
            !FGuid::ParseExact(CommandText, EGuidFormats::Digits, CommandId))
        {
            OutError = TEXT("Invalid committed command ID.");
            return false;
        }
        Candidate.Simulation.CommittedCommandIds.Add(CommandId);
    }

    TSet<FString> StreamIds;
    for (const FTruongSinhRngStreamState& Stream : Candidate.Simulation.Rng.Streams)
    {
        if (StreamIds.Contains(Stream.StreamId))
        {
            OutError = TEXT("Duplicate RNG stream ID.");
            return false;
        }
        StreamIds.Add(Stream.StreamId);
    }
    TSet<FGuid> CommandIds;
    for (const FGuid& CommandId : Candidate.Simulation.CommittedCommandIds)
    {
        if (CommandIds.Contains(CommandId))
        {
            OutError = TEXT("Duplicate committed command ID.");
            return false;
        }
        CommandIds.Add(CommandId);
    }

    if (Candidate.Simulation.SchemaVersion != 1 || Candidate.Simulation.WorldRevision < 0 ||
        Candidate.Simulation.ElapsedDays < 0 || Candidate.Simulation.Rng.AlgorithmVersion != 1)
    {
        OutError = TEXT("Unsupported or invalid simulation state.");
        return false;
    }
    if (FTruongSinhGameSimulation::ComputeStateHash(Candidate.Simulation) != Candidate.PayloadHash)
    {
        OutError = TEXT("Payload hash mismatch.");
        return false;
    }

    OutSave = MoveTemp(Candidate);
    return true;
}
