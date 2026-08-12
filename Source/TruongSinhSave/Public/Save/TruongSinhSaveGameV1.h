#pragma once

#include "CoreMinimal.h"
#include "Simulation/TruongSinhGameSimulation.h"
#include "TruongSinhSaveGameV1.generated.h"

/** First versioned save envelope. Later domains extend payload through explicit migrations. */
USTRUCT()
struct TRUONGSINHSAVE_API FTruongSinhSaveGameV1
{
    GENERATED_BODY()

    static constexpr int32 CurrentSchemaVersion = 1;

    UPROPERTY()
    int32 SaveSchemaVersion = CurrentSchemaVersion;

    UPROPERTY()
    FString GameBuildId = TEXT("dev-unverified");

    UPROPERTY()
    FTruongSinhSimulationState Simulation;

    /** Hash of the canonical simulation payload, checked before applying a load. */
    UPROPERTY()
    FString PayloadHash;
};

/** Deterministic JSON transport. File backup/atomic replacement is added after Windows platform tests. */
class TRUONGSINHSAVE_API FTruongSinhSaveJsonCodec
{
public:
    static bool Serialize(const FTruongSinhSaveGameV1& Save, FString& OutJson, FString& OutError);
    static bool Deserialize(const FString& Json, FTruongSinhSaveGameV1& OutSave, FString& OutError);
};
