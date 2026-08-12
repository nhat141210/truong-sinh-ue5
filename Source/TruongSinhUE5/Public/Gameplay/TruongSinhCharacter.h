#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "TruongSinhCharacter.generated.h"

class UCameraComponent;
class UAnimationAsset;
class UInputAction;
class UInputMappingContext;
class USpringArmComponent;
struct FInputActionValue;

/**
 * Third-person exploration shell with a temporary, provenanced UE mannequin.
 * Character-art passes may replace presentation without moving authority here.
 */
UCLASS()
class TRUONGSINHUE5_API ATruongSinhCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    ATruongSinhCharacter();

    virtual void BeginPlay() override;
    virtual void Tick(float DeltaSeconds) override;
    virtual void PawnClientRestart() override;
    virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
    TObjectPtr<USpringArmComponent> CameraBoom;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
    TObjectPtr<UCameraComponent> FollowCamera;

private:
    UPROPERTY(Transient)
    TObjectPtr<UAnimationAsset> IdleAnimation;

    UPROPERTY(Transient)
    TObjectPtr<UAnimationAsset> JogAnimation;

    UPROPERTY(Transient)
    TObjectPtr<UAnimationAsset> FallAnimation;

    UPROPERTY(Transient)
    TObjectPtr<UAnimationAsset> ActiveAnimation;

    UPROPERTY(Transient)
    TObjectPtr<UInputMappingContext> ExplorationMappingContext;

    UPROPERTY(Transient)
    TObjectPtr<UInputAction> MoveForwardAction;

    UPROPERTY(Transient)
    TObjectPtr<UInputAction> MoveBackwardAction;

    UPROPERTY(Transient)
    TObjectPtr<UInputAction> MoveLeftAction;

    UPROPERTY(Transient)
    TObjectPtr<UInputAction> MoveRightAction;

    UPROPERTY(Transient)
    TObjectPtr<UInputAction> LookYawAction;

    UPROPERTY(Transient)
    TObjectPtr<UInputAction> LookPitchAction;

    UPROPERTY(Transient)
    TObjectPtr<UInputAction> JumpAction;

    UPROPERTY(Transient)
    TObjectPtr<UInputAction> InteractAction;

    UPROPERTY(Transient)
    TObjectPtr<UInputAction> PauseAction;

    UPROPERTY(Transient)
    TObjectPtr<UInputAction> ConflictApproach1Action;

    UPROPERTY(Transient)
    TObjectPtr<UInputAction> ConflictApproach2Action;

    UPROPERTY(Transient)
    TObjectPtr<UInputAction> ConflictApproach3Action;

    UPROPERTY(Transient)
    TObjectPtr<UInputAction> ConflictApproach4Action;

    UPROPERTY(Transient)
    TObjectPtr<UInputAction> ConflictApproach5Action;

    void MoveForward(const FInputActionValue& Value);
    void MoveBackward(const FInputActionValue& Value);
    void MoveLeft(const FInputActionValue& Value);
    void MoveRight(const FInputActionValue& Value);
    void LookYaw(const FInputActionValue& Value);
    void LookPitch(const FInputActionValue& Value);
    void Interact();
    void TogglePause();
    void SelectConflictApproach1();
    void SelectConflictApproach2();
    void SelectConflictApproach3();
    void SelectConflictApproach4();
    void SelectConflictApproach5();
};
