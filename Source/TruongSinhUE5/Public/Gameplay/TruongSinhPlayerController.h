#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "TruongSinhPlayerController.generated.h"

class UTruongSinhRuntimeHUDWidget;
class UTruongSinhActivityRegistryDataAsset;
class AActor;
struct FTruongSinhInteractionOffer;

UCLASS()
class TRUONGSINHUE5_API ATruongSinhPlayerController : public APlayerController
{
    GENERATED_BODY()

public:
    ATruongSinhPlayerController(const FObjectInitializer& ObjectInitializer);
    virtual void BeginPlay() override;
    virtual void PlayerTick(float DeltaTime) override;

    void TryInteract();
    void SelectConflictApproach(int32 OptionIndex);
    void TogglePauseMenu();

private:
    UPROPERTY(Transient)
    TObjectPtr<UTruongSinhRuntimeHUDWidget> RuntimeHUD;

    UPROPERTY(Transient)
    TObjectPtr<UTruongSinhActivityRegistryDataAsset> ActivityRegistry;

    FString PendingReplayId;
    bool bConflictPlannerOpen = false;
    int32 SelectedConflictApproachIndex = INDEX_NONE;
    TArray<bool> ConflictApproachEligibility;

    void ApplyGameplayMouseCapture();
    void CloseConflictPlanner();
    bool FindBestInteraction(AActor*& OutProvider, FTruongSinhInteractionOffer& OutOffer) const;
    bool SaveCanonicalState(FString& OutError) const;
    bool RestoreCanonicalState(FString& OutError);
};
