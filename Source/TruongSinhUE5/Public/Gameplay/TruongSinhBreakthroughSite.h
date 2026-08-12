#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TruongSinhInteractionProvider.h"
#include "TruongSinhBreakthroughSite.generated.h"

class UPointLightComponent;
class USceneComponent;
class UStaticMeshComponent;

/**
 * World adapter for the first shared-framework breakthrough activity. Like the
 * cultivation site, it only offers a typed command; it never resolves or
 * mutates canonical state.
 */
UCLASS(BlueprintType)
class TRUONGSINHUE5_API ATruongSinhBreakthroughSite final
    : public AActor
    , public ITruongSinhInteractionProvider
{
    GENERATED_BODY()

public:
    ATruongSinhBreakthroughSite();

    virtual TArray<FTruongSinhInteractionOffer> GetInteractionOffers_Implementation(
        FTruongSinhStableId InstigatorId) const override;

    virtual FTruongSinhActionCommand BuildInteractionCommand_Implementation(
        FTruongSinhStableId CandidateId,
        FTruongSinhStableId InstigatorId,
        int64 ExpectedWorldRevision,
        int64 Sequence) const override;

private:
    UPROPERTY(VisibleAnywhere, Category = "Truong Sinh|Breakthrough")
    TObjectPtr<USceneComponent> SceneRoot;

    UPROPERTY(VisibleAnywhere, Category = "Truong Sinh|Breakthrough")
    TObjectPtr<UStaticMeshComponent> DaisMesh;

    UPROPERTY(VisibleAnywhere, Category = "Truong Sinh|Breakthrough")
    TObjectPtr<UStaticMeshComponent> BeaconMesh;

    UPROPERTY(VisibleAnywhere, Category = "Truong Sinh|Breakthrough")
    TObjectPtr<UPointLightComponent> BeaconLight;
};
