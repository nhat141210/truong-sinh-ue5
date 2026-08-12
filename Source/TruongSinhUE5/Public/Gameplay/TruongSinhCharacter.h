#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "TruongSinhCharacter.generated.h"

class UCameraComponent;
class USpringArmComponent;

/**
 * Temporary M1 third-person shell. The Windows vertical slice replaces its
 * presentation mesh and locomotion without moving gameplay authority here.
 */
UCLASS()
class TRUONGSINHUE5_API ATruongSinhCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    ATruongSinhCharacter();

    virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
    TObjectPtr<USpringArmComponent> CameraBoom;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
    TObjectPtr<UCameraComponent> FollowCamera;

private:
    void MoveForward(float Value);
    void MoveRight(float Value);
    void Turn(float Value);
    void LookUp(float Value);
};
