#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TruongSinhRuntimeHUDWidget.generated.h"

class UBorder;
class UOverlay;
class UTextBlock;
class UTexture2D;

/**
 * Native HUD for the M2A playable loop.
 *
 * This widget deliberately presents canonical state without owning or mutating it.
 * It provides the canonical presentation surface used by smoke and standalone validation.
 */
UCLASS()
class TRUONGSINHUE5_API UTruongSinhRuntimeHUDWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    UTruongSinhRuntimeHUDWidget(const FObjectInitializer& ObjectInitializer);

    UFUNCTION(BlueprintCallable, Category = "Truong Sinh|UI")
    void SetInteractionPrompt(const FText& Prompt, bool bVisible = true);

    UFUNCTION(BlueprintCallable, Category = "Truong Sinh|UI")
    void ShowActivityResult(const FText& ResultTitle, const FText& ResultDetails, bool bSuccess = true);

    UFUNCTION(BlueprintCallable, Category = "Truong Sinh|UI")
    void SetPaused(bool bPaused);

    /** Eligibility copy is supplied by the controller; this widget never evaluates gameplay rules. */
    void ShowConflictPlanner(const TArray<FText>& EligibilityLines);
    void HideConflictPlanner();

    /** Pulls read-only values from the canonical simulation facade. */
    UFUNCTION(BlueprintCallable, Category = "Truong Sinh|UI")
    void RefreshState();

protected:
    virtual TSharedRef<SWidget> RebuildWidget() override;
    virtual void NativeConstruct() override;

private:
    UPROPERTY(Transient)
    TObjectPtr<UTextBlock> RealmText;

    UPROPERTY(Transient)
    TObjectPtr<UTextBlock> CultivationText;

    UPROPERTY(Transient)
    TObjectPtr<UTextBlock> TimeText;

    UPROPERTY(Transient)
    TObjectPtr<UTextBlock> RevisionText;

    UPROPERTY(Transient)
    TObjectPtr<UBorder> InteractionPanel;

    UPROPERTY(Transient)
    TObjectPtr<UTextBlock> InteractionText;

    UPROPERTY(Transient)
    TObjectPtr<UBorder> ResultPanel;

    UPROPERTY(Transient)
    TObjectPtr<UOverlay> ResultFrameRoot;

    UPROPERTY(Transient)
    TObjectPtr<UTextBlock> ResultTitleText;

    UPROPERTY(Transient)
    TObjectPtr<UTextBlock> ResultDetailsText;

    UPROPERTY(Transient)
    TObjectPtr<UBorder> ResultAccent;

    UPROPERTY(Transient)
    TObjectPtr<UBorder> PauseOverlay;

    UPROPERTY(Transient)
    TObjectPtr<UOverlay> ConflictPlannerRoot;

    UPROPERTY(Transient)
    TArray<TObjectPtr<UTextBlock>> ConflictOptionTexts;

    UPROPERTY(EditDefaultsOnly, Category = "Truong Sinh|UI")
    TObjectPtr<UTexture2D> OrnateFrameTexture;
};
