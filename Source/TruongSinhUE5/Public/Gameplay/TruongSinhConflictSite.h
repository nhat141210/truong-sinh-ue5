#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TruongSinhInteractionProvider.h"
#include "TruongSinhConflictSite.generated.h"

class UPointLightComponent;
class USceneComponent;
class UStaticMeshComponent;

/** World adapter for one deterministic, non-action-combat encounter. */
UCLASS(BlueprintType)
class TRUONGSINHUE5_API ATruongSinhConflictSite final : public AActor, public ITruongSinhInteractionProvider
{
    GENERATED_BODY()

public:
    ATruongSinhConflictSite();
    virtual TArray<FTruongSinhInteractionOffer> GetInteractionOffers_Implementation(FTruongSinhStableId InstigatorId) const override;
    virtual FTruongSinhActionCommand BuildInteractionCommand_Implementation(
        FTruongSinhStableId CandidateId, FTruongSinhStableId InstigatorId,
        int64 ExpectedWorldRevision, int64 Sequence) const override;

private:
    UPROPERTY(VisibleAnywhere, Category = "Truong Sinh|Conflict") TObjectPtr<USceneComponent> SceneRoot;
    UPROPERTY(VisibleAnywhere, Category = "Truong Sinh|Conflict") TObjectPtr<UStaticMeshComponent> TrialStone;
    UPROPERTY(VisibleAnywhere, Category = "Truong Sinh|Conflict") TObjectPtr<UStaticMeshComponent> OpponentMarker;
    UPROPERTY(VisibleAnywhere, Category = "Truong Sinh|Conflict") TObjectPtr<UPointLightComponent> TrialLight;
};
