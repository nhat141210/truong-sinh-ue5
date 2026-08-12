#pragma once

#include "CoreMinimal.h"
#include "Core/TruongSinhTypes.h"
#include "Engine/DataAsset.h"
#include "TruongSinhActivityRegistryDataAsset.generated.h"

/** Authored, non-mutable description consumed by the shared activity pipeline. */
USTRUCT(BlueprintType)
struct TRUONGSINHDATA_API FTruongSinhActivityDefinition
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Truong Sinh|Activity")
    FTruongSinhStableId ActivityId;

    /** The world interaction offer that selects this definition. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Truong Sinh|Activity")
    FTruongSinhStableId FacilityId;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Truong Sinh|Activity")
    FTruongSinhStableId MethodId;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Truong Sinh|Activity")
    FTruongSinhStableId LocationId;

    /** Stable resolver registration key, never a display string. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Truong Sinh|Activity")
    FName ResolverId = NAME_None;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Truong Sinh|Activity", meta = (ClampMin = "1"))
    int64 DurationMinutes = 1;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Truong Sinh|Activity", meta = (ClampMin = "0"))
    int64 MinimumCultivationUnits = 0;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Truong Sinh|Activity", meta = (ClampMin = "0"))
    int64 DifficultyOrTargetPower = 0;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Truong Sinh|Activity")
    int32 TechniqueModifierUnits = 0;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Truong Sinh|Activity")
    int32 PreparationModifierUnits = 0;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Truong Sinh|Activity")
    int32 EnvironmentModifierUnits = 0;

    /** Optional canonical output for recipes such as alchemy. Empty means no inventory output. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Truong Sinh|Activity")
    FTruongSinhStableId OutputId;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Truong Sinh|Activity", meta = (ClampMin = "0"))
    int64 MaximumOutputUnits = 0;

    /** Optional canonical formation effect. Empty for non-formation activities. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Truong Sinh|Activity")
    FTruongSinhStableId FormationEffectId;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Truong Sinh|Activity", meta = (ClampMin = "0"))
    int64 FormationDurationMinutes = 0;

    /** Canonical opponent selected by an authored conflict encounter. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Truong Sinh|Activity")
    FTruongSinhStableId ConflictOpponentId;
};

/**
 * Immutable source-of-truth for activity setup. It owns no save state and does
 * not resolve or commit gameplay; those remain in Resolution and Simulation.
 */
UCLASS(BlueprintType)
class TRUONGSINHDATA_API UTruongSinhActivityRegistryDataAsset final : public UDataAsset
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Truong Sinh|Activity")
    TArray<FTruongSinhActivityDefinition> Definitions;

    const FTruongSinhActivityDefinition* FindByFacility(const FTruongSinhStableId& FacilityId) const;
    bool ValidateRegistry(FString& OutError) const;
};
