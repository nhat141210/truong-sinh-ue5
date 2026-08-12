#pragma once

#include "CoreMinimal.h"
#include "Core/TruongSinhTypes.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "TruongSinhActivityResolution.generated.h"

UENUM(BlueprintType)
enum class ETruongSinhActivityType : uint8
{
    Cultivation,
    Breakthrough,
    Alchemy,
    Formation,
    Conflict
};

UENUM(BlueprintType)
enum class ETruongSinhActivityStrategy : uint8
{
    Safe,
    Balanced,
    Forceful,
    Overwhelm,
    Cautious,
    Endure,
    Retreat
};

UENUM(BlueprintType)
enum class ETruongSinhResolutionOutcome : uint8
{
    Rejected,
    Failure,
    PartialSuccess,
    Success,
    GreatSuccess
};

USTRUCT(BlueprintType)
struct TRUONGSINHRESOLUTION_API FTruongSinhActivityResource
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Truong Sinh|Activity")
    FTruongSinhStableId ResourceId;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Truong Sinh|Activity", meta = (ClampMin = "1"))
    int64 Units = 1;
};

/** Player-authored preparation. It contains choices, never calculated outcomes. */
USTRUCT(BlueprintType)
struct TRUONGSINHRESOLUTION_API FTruongSinhActivityPlan
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Truong Sinh|Activity")
    FTruongSinhActionCommand Action;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Truong Sinh|Activity")
    ETruongSinhActivityType Type = ETruongSinhActivityType::Cultivation;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Truong Sinh|Activity")
    FTruongSinhStableId ActivityId;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Truong Sinh|Activity")
    FTruongSinhStableId MethodId;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Truong Sinh|Activity")
    FTruongSinhStableId FacilityId;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Truong Sinh|Activity")
    FTruongSinhStableId LocationId;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Truong Sinh|Activity")
    TArray<FTruongSinhActivityResource> Resources;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Truong Sinh|Activity", meta = (ClampMin = "1"))
    int64 DurationMinutes = 1;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Truong Sinh|Activity")
    ETruongSinhActivityStrategy Strategy = ETruongSinhActivityStrategy::Balanced;
};

/** Immutable canonical values supplied by simulation. Every score uses the same fixed-point units. */
USTRUCT(BlueprintType)
struct TRUONGSINHRESOLUTION_API FTruongSinhActivitySnapshot
{
    GENERATED_BODY()

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Truong Sinh|Activity")
    int64 PerformerPower = 0;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Truong Sinh|Activity")
    int64 DifficultyOrTargetPower = 0;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Truong Sinh|Activity")
    int32 TechniqueModifierUnits = 0;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Truong Sinh|Activity")
    int32 PreparationModifierUnits = 0;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Truong Sinh|Activity")
    int32 EnvironmentModifierUnits = 0;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Truong Sinh|Activity")
    int64 MasterSeed = 0;
};

USTRUCT(BlueprintType)
struct TRUONGSINHRESOLUTION_API FTruongSinhResolutionFactor
{
    GENERATED_BODY()

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Truong Sinh|Activity")
    FTruongSinhStableId FactorId;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Truong Sinh|Activity")
    int64 Units = 0;
};

/** Presentation hint emitted after rules are resolved; it has no gameplay authority. */
USTRUCT(BlueprintType)
struct TRUONGSINHRESOLUTION_API FTruongSinhPresentationBeat
{
    GENERATED_BODY()

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Truong Sinh|Presentation")
    int32 Sequence = 0;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Truong Sinh|Presentation")
    FTruongSinhStableId BeatId;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Truong Sinh|Presentation")
    FTruongSinhStableId PresentationCueId;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Truong Sinh|Presentation")
    int64 MagnitudeUnits = 0;
};

USTRUCT(BlueprintType)
struct TRUONGSINHRESOLUTION_API FTruongSinhActivityPreview
{
    GENERATED_BODY()

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Truong Sinh|Activity")
    bool bEligible = false;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Truong Sinh|Activity")
    FTruongSinhStableId ReasonId;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Truong Sinh|Activity")
    FTruongSinhStableId RiskBandId;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Truong Sinh|Activity")
    int64 DurationMinutes = 0;
};

/** Deterministic draft. Simulation commits its deltas before presentation consumes Beats. */
USTRUCT(BlueprintType)
struct TRUONGSINHRESOLUTION_API FTruongSinhAutoResolutionResult
{
    GENERATED_BODY()

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Truong Sinh|Activity")
    ETruongSinhResolutionOutcome Outcome = ETruongSinhResolutionOutcome::Rejected;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Truong Sinh|Activity")
    FTruongSinhStableId ReasonId;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Truong Sinh|Activity")
    int64 FinalScore = 0;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Truong Sinh|Activity")
    int64 TargetScore = 0;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Truong Sinh|Activity")
    int64 TimeAdvancedMinutes = 0;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Truong Sinh|Activity")
    TArray<FTruongSinhResolutionFactor> Factors;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Truong Sinh|Activity")
    TArray<FTruongSinhPresentationBeat> Beats;
};

/** Pure resolver proof. It never accesses UWorld, frame time, actors, UI or presentation assets. */
class TRUONGSINHRESOLUTION_API FTruongSinhAutoResolver
{
public:
    static FTruongSinhActivityPreview Preview(
        const FTruongSinhActivitySnapshot& Snapshot,
        const FTruongSinhActivityPlan& Plan);

    static FTruongSinhAutoResolutionResult Resolve(
        const FTruongSinhActivitySnapshot& Snapshot,
        const FTruongSinhActivityPlan& Plan);
};

/** Blueprint may request non-random previews. Canonical resolution remains a simulation concern. */
UCLASS()
class TRUONGSINHRESOLUTION_API UTruongSinhAutoResolutionSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintPure, Category = "Truong Sinh|Activity")
    FTruongSinhActivityPreview PreviewActivity(
        const FTruongSinhActivitySnapshot& Snapshot,
        const FTruongSinhActivityPlan& Plan) const;
};
