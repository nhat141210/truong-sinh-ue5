#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TruongSinhInteractionProvider.h"
#include "TruongSinhCultivationSite.generated.h"

class UPointLightComponent;
class USceneComponent;
class UStaticMeshComponent;

/**
 * A small world-facing adapter for the M2A cultivation loop.
 * It authors an interaction offer and a canonical command, but never mutates simulation state.
 */
UCLASS(BlueprintType)
class TRUONGSINHUE5_API ATruongSinhCultivationSite final
    : public AActor
    , public ITruongSinhInteractionProvider
{
    GENERATED_BODY()

public:
    ATruongSinhCultivationSite();

    virtual TArray<FTruongSinhInteractionOffer> GetInteractionOffers_Implementation(
        FTruongSinhStableId InstigatorId) const override;

    virtual FTruongSinhActionCommand BuildInteractionCommand_Implementation(
        FTruongSinhStableId CandidateId,
        FTruongSinhStableId InstigatorId,
        int64 ExpectedWorldRevision,
        int64 Sequence) const override;

private:
    UPROPERTY(VisibleAnywhere, Category = "Truong Sinh|Cultivation")
    TObjectPtr<USceneComponent> SceneRoot;

    UPROPERTY(VisibleAnywhere, Category = "Truong Sinh|Cultivation")
    TObjectPtr<UStaticMeshComponent> AltarMesh;

    UPROPERTY(VisibleAnywhere, Category = "Truong Sinh|Cultivation")
    TObjectPtr<UStaticMeshComponent> FocusMesh;

    UPROPERTY(VisibleAnywhere, Category = "Truong Sinh|Cultivation")
    TObjectPtr<UPointLightComponent> FocusLight;
};
