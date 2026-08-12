#pragma once

#include "CoreMinimal.h"
#include "Core/TruongSinhTypes.h"
#include "UObject/Interface.h"
#include "TruongSinhInteractionProvider.generated.h"

USTRUCT(BlueprintType)
struct TRUONGSINHWORLD_API FTruongSinhInteractionOffer
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Truong Sinh|Interaction")
    FTruongSinhStableId CandidateId;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Truong Sinh|Interaction")
    FName PromptStringKey = NAME_None;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Truong Sinh|Interaction")
    FTruongSinhStableId DisabledReasonId;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Truong Sinh|Interaction")
    int32 Priority = 0;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Truong Sinh|Interaction", meta = (ClampMin = "0.0"))
    float MaximumRangeCentimeters = 225.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Truong Sinh|Interaction")
    bool bEnabled = true;
};

/** Pure, deterministic offer selection shared by runtime targeting and automation. */
class TRUONGSINHWORLD_API FTruongSinhInteractionSelection
{
public:
    static bool SelectBestOffer(
        const TArray<FTruongSinhInteractionOffer>& Offers,
        float DistanceCentimeters,
        FTruongSinhInteractionOffer& OutOffer);
};

/** World actors offer commands; only the application facade may validate and commit them. */
UINTERFACE(MinimalAPI, Blueprintable)
class UTruongSinhInteractionProvider : public UInterface
{
    GENERATED_BODY()
};

class TRUONGSINHWORLD_API ITruongSinhInteractionProvider
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Truong Sinh|Interaction")
    TArray<FTruongSinhInteractionOffer> GetInteractionOffers(FTruongSinhStableId InstigatorId) const;

    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Truong Sinh|Interaction")
    FTruongSinhActionCommand BuildInteractionCommand(
        FTruongSinhStableId CandidateId,
        FTruongSinhStableId InstigatorId,
        int64 ExpectedWorldRevision,
        int64 Sequence) const;
};
