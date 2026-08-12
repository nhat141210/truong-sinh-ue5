#pragma once

#include "CoreMinimal.h"
#include "Simulation/TruongSinhGameSimulation.h"
#include "TruongSinhSaveGameV2.generated.h"

/** First sandbox save envelope. No commercial v1 save was released. */
USTRUCT()
struct TRUONGSINHSAVE_API FTruongSinhSaveGameV2
{
    GENERATED_BODY()

    static constexpr int32 CurrentSchemaVersion = 2;

    UPROPERTY()
    int32 SaveSchemaVersion = CurrentSchemaVersion;

    UPROPERTY()
    FString GameBuildId = TEXT("dev-unverified");

    UPROPERTY()
    FTruongSinhSimulationState Simulation;

    /** Hash of canonical simulation state, checked before any load is applied. */
    UPROPERTY()
    FString PayloadHash;

    /** Presentation-only replay ID; result is already committed and cannot reroll. */
    UPROPERTY()
    FTruongSinhStableId PendingReplayId;
};

/** Deterministic JSON transport. Atomic file/backup remains a Windows filesystem gate. */
class TRUONGSINHSAVE_API FTruongSinhSaveJsonCodecV2
{
public:
    static bool Serialize(const FTruongSinhSaveGameV2& Save, FString& OutJson, FString& OutError);
    static bool Deserialize(const FString& Json, FTruongSinhSaveGameV2& OutSave, FString& OutError);
};
