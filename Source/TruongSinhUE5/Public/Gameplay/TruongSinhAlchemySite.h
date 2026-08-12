#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TruongSinhInteractionProvider.h"
#include "TruongSinhAlchemySite.generated.h"

class UPointLightComponent;
class USceneComponent;
class UStaticMeshComponent;

/** World adapter for the shared alchemy recipe activity; never owns its outcome. */
UCLASS(BlueprintType)
class TRUONGSINHUE5_API ATruongSinhAlchemySite final : public AActor, public ITruongSinhInteractionProvider
{
    GENERATED_BODY()

public:
    ATruongSinhAlchemySite();
    virtual TArray<FTruongSinhInteractionOffer> GetInteractionOffers_Implementation(
        FTruongSinhStableId InstigatorId) const override;
    virtual FTruongSinhActionCommand BuildInteractionCommand_Implementation(
        FTruongSinhStableId CandidateId, FTruongSinhStableId InstigatorId,
        int64 ExpectedWorldRevision, int64 Sequence) const override;

private:
    UPROPERTY(VisibleAnywhere, Category = "Truong Sinh|Alchemy") TObjectPtr<USceneComponent> SceneRoot;
    UPROPERTY(VisibleAnywhere, Category = "Truong Sinh|Alchemy") TObjectPtr<UStaticMeshComponent> FurnaceMesh;
    UPROPERTY(VisibleAnywhere, Category = "Truong Sinh|Alchemy") TObjectPtr<UStaticMeshComponent> FlameMesh;
    UPROPERTY(VisibleAnywhere, Category = "Truong Sinh|Alchemy") TObjectPtr<UPointLightComponent> FurnaceLight;
};
