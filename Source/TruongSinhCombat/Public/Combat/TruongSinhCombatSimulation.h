#pragma once

#include "CoreMinimal.h"
#include "Core/TruongSinhTypes.h"
#include "TruongSinhCombatSimulation.generated.h"

/** Pure turn-based combat input, independent from targets selected in 3D. */
USTRUCT(BlueprintType)
struct TRUONGSINHCOMBAT_API FTruongSinhCombatCommand
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Truong Sinh|Combat")
    FTruongSinhActionCommand Action;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Truong Sinh|Combat")
    FTruongSinhStableId TechniqueId;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Truong Sinh|Combat")
    TArray<FTruongSinhStableId> TargetIds;
};

USTRUCT(BlueprintType)
struct TRUONGSINHCOMBAT_API FTruongSinhCombatValue
{
    GENERATED_BODY()

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Truong Sinh|Combat")
    FTruongSinhStableId ValueTypeId;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Truong Sinh|Combat")
    int64 Units = 0;
};

/** A structured event is replayed by the arena presentation after resolution. */
USTRUCT(BlueprintType)
struct TRUONGSINHCOMBAT_API FTruongSinhCombatReplayEvent
{
    GENERATED_BODY()

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Truong Sinh|Combat")
    FTruongSinhStableId EventId;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Truong Sinh|Combat")
    FTruongSinhStableId SourceId;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Truong Sinh|Combat")
    FTruongSinhStableId TargetId;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Truong Sinh|Combat")
    FTruongSinhStableId PresentationCueId;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Truong Sinh|Combat")
    int64 Sequence = 0;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Truong Sinh|Combat")
    TArray<FTruongSinhCombatValue> QuantizedValues;
};

USTRUCT(BlueprintType)
struct TRUONGSINHCOMBAT_API FTruongSinhElementPool
{
    GENERATED_BODY()

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Truong Sinh|Combat")
    ETruongSinhElement Element = ETruongSinhElement::Metal;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Truong Sinh|Combat")
    int64 Units = 0;
};

/** Immutable participant state supplied to the pure combat resolver. */
USTRUCT(BlueprintType)
struct TRUONGSINHCOMBAT_API FTruongSinhCombatantSnapshot
{
    GENERATED_BODY()

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Truong Sinh|Combat")
    FTruongSinhStableId CombatantId;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Truong Sinh|Combat")
    int64 HealthUnits = 0;

    /** Must be serialized in ETruongSinhElement ordinal order. */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Truong Sinh|Combat")
    TArray<FTruongSinhElementPool> ElementPools;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Truong Sinh|Combat")
    TArray<FTruongSinhStableId> ActiveEffectIds;
};

/**
 * Pure simulation boundary. Implementations must be deterministic from the
 * snapshot, command and RNG state; they may not access UWorld, UI, animation
 * or presentation cues.
 */
USTRUCT(BlueprintType)
struct TRUONGSINHCOMBAT_API FTruongSinhCombatSnapshot
{
    GENERATED_BODY()

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Truong Sinh|Combat")
    FTruongSinhStableId EncounterId;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Truong Sinh|Combat")
    int64 CombatRevision = 0;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Truong Sinh|Combat")
    int32 TurnIndex = 0;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Truong Sinh|Combat")
    FTruongSinhStableId ActingCombatantId;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Truong Sinh|Combat")
    TArray<FTruongSinhCombatantSnapshot> Combatants;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Truong Sinh|Combat")
    FTruongSinhRngState Rng;
};

USTRUCT(BlueprintType)
struct TRUONGSINHCOMBAT_API FTruongSinhCombatResult
{
    GENERATED_BODY()

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Truong Sinh|Combat")
    bool bCommitted = false;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Truong Sinh|Combat")
    FTruongSinhActionResult ActionResult;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Truong Sinh|Combat")
    TArray<FTruongSinhCombatReplayEvent> ReplayEvents;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Truong Sinh|Combat")
    FTruongSinhCombatSnapshot FinalSnapshot;
};

class TRUONGSINHCOMBAT_API ITruongSinhCombatSimulation
{
public:
    virtual ~ITruongSinhCombatSimulation() = default;
    virtual FTruongSinhCombatResult Resolve(
        const FTruongSinhCombatSnapshot& Snapshot,
        const FTruongSinhCombatCommand& Command) = 0;
};
