#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "TruongSinhPlayerController.generated.h"

class UTruongSinhRuntimeHUDWidget;
class AActor;
struct FTruongSinhInteractionOffer;

UCLASS()
class TRUONGSINHUE5_API ATruongSinhPlayerController : public APlayerController
{
    GENERATED_BODY()

public:
    virtual void BeginPlay() override;
    virtual void PlayerTick(float DeltaTime) override;

    void TryInteract();
    void TogglePauseMenu();

private:
    UPROPERTY(Transient)
    TObjectPtr<UTruongSinhRuntimeHUDWidget> RuntimeHUD;

    FString PendingReplayId;

    void ApplyGameplayMouseCapture();
    bool FindBestInteraction(AActor*& OutProvider, FTruongSinhInteractionOffer& OutOffer) const;
    bool SaveCanonicalState(FString& OutError) const;
    bool RestoreCanonicalState(FString& OutError);
};
