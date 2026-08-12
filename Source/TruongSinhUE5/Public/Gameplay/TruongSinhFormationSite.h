#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TruongSinhInteractionProvider.h"
#include "TruongSinhFormationSite.generated.h"

class UPointLightComponent;
class USceneComponent;
class UStaticMeshComponent;

/** World adapter for installing the first deterministic formation. */
UCLASS(BlueprintType)
class TRUONGSINHUE5_API ATruongSinhFormationSite final : public AActor, public ITruongSinhInteractionProvider
{
    GENERATED_BODY()

public:
    ATruongSinhFormationSite();
    virtual TArray<FTruongSinhInteractionOffer> GetInteractionOffers_Implementation(
        FTruongSinhStableId InstigatorId) const override;
    virtual FTruongSinhActionCommand BuildInteractionCommand_Implementation(
        FTruongSinhStableId CandidateId, FTruongSinhStableId InstigatorId,
        int64 ExpectedWorldRevision, int64 Sequence) const override;

private:
    UPROPERTY(VisibleAnywhere, Category = "Truong Sinh|Formation") TObjectPtr<USceneComponent> SceneRoot;
    UPROPERTY(VisibleAnywhere, Category = "Truong Sinh|Formation") TObjectPtr<UStaticMeshComponent> ArrayPlate;
    UPROPERTY(VisibleAnywhere, Category = "Truong Sinh|Formation") TObjectPtr<UStaticMeshComponent> AnchorA;
    UPROPERTY(VisibleAnywhere, Category = "Truong Sinh|Formation") TObjectPtr<UStaticMeshComponent> AnchorB;
    UPROPERTY(VisibleAnywhere, Category = "Truong Sinh|Formation") TObjectPtr<UStaticMeshComponent> AnchorC;
    UPROPERTY(VisibleAnywhere, Category = "Truong Sinh|Formation") TObjectPtr<UPointLightComponent> ArrayLight;
};
