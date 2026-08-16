#include "Gameplay/TruongSinhCharacter.h"

#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "GameFramework/Controller.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Gameplay/TruongSinhPlayerController.h"
#include "InputAction.h"
#include "InputActionValue.h"
#include "InputCoreTypes.h"
#include "InputMappingContext.h"
#include "Math/RotationMatrix.h"

ATruongSinhCharacter::ATruongSinhCharacter()
{
    PrimaryActorTick.bCanEverTick = true;

    GetCapsuleComponent()->InitCapsuleSize(42.0f, 96.0f);

    bUseControllerRotationPitch = false;
    bUseControllerRotationYaw = false;
    bUseControllerRotationRoll = false;

    GetCharacterMovement()->bOrientRotationToMovement = true;
    GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);

    // Presentation assets are intentionally assigned later by the clean
    // content pass. Gameplay movement and input must not depend on a bundled
    // character mesh or animation asset.
    GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
    CameraBoom->SetupAttachment(RootComponent);
    CameraBoom->TargetArmLength = 410.0f;
    CameraBoom->SocketOffset = FVector(0.0f, 58.0f, 34.0f);
    CameraBoom->bEnableCameraLag = true;
    CameraBoom->CameraLagSpeed = 12.0f;
    CameraBoom->CameraLagMaxDistance = 35.0f;
    CameraBoom->bEnableCameraRotationLag = true;
    CameraBoom->CameraRotationLagSpeed = 16.0f;
    CameraBoom->bUsePawnControlRotation = true;

    FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
    FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
    FollowCamera->bUsePawnControlRotation = false;
    FollowCamera->SetFieldOfView(75.0f);

    ExplorationMappingContext = CreateDefaultSubobject<UInputMappingContext>(TEXT("IMC_Exploration"));
    MoveForwardAction = CreateDefaultSubobject<UInputAction>(TEXT("IA_MoveForward"));
    MoveBackwardAction = CreateDefaultSubobject<UInputAction>(TEXT("IA_MoveBackward"));
    MoveLeftAction = CreateDefaultSubobject<UInputAction>(TEXT("IA_MoveLeft"));
    MoveRightAction = CreateDefaultSubobject<UInputAction>(TEXT("IA_MoveRight"));
    LookYawAction = CreateDefaultSubobject<UInputAction>(TEXT("IA_LookYaw"));
    LookPitchAction = CreateDefaultSubobject<UInputAction>(TEXT("IA_LookPitch"));
    JumpAction = CreateDefaultSubobject<UInputAction>(TEXT("IA_Jump"));
    InteractAction = CreateDefaultSubobject<UInputAction>(TEXT("IA_Interact"));
    PauseAction = CreateDefaultSubobject<UInputAction>(TEXT("IA_Pause"));
    ConflictApproach1Action = CreateDefaultSubobject<UInputAction>(TEXT("IA_ConflictApproach1"));
    ConflictApproach2Action = CreateDefaultSubobject<UInputAction>(TEXT("IA_ConflictApproach2"));
    ConflictApproach3Action = CreateDefaultSubobject<UInputAction>(TEXT("IA_ConflictApproach3"));
    ConflictApproach4Action = CreateDefaultSubobject<UInputAction>(TEXT("IA_ConflictApproach4"));
    ConflictApproach5Action = CreateDefaultSubobject<UInputAction>(TEXT("IA_ConflictApproach5"));

    LookYawAction->ValueType = EInputActionValueType::Axis1D;
    LookPitchAction->ValueType = EInputActionValueType::Axis1D;
    PauseAction->bTriggerWhenPaused = true;

    ExplorationMappingContext->MapKey(MoveForwardAction, EKeys::W);
    ExplorationMappingContext->MapKey(MoveBackwardAction, EKeys::S);
    ExplorationMappingContext->MapKey(MoveLeftAction, EKeys::A);
    ExplorationMappingContext->MapKey(MoveRightAction, EKeys::D);
    ExplorationMappingContext->MapKey(LookYawAction, EKeys::MouseX);
    ExplorationMappingContext->MapKey(LookPitchAction, EKeys::MouseY);
    ExplorationMappingContext->MapKey(JumpAction, EKeys::SpaceBar);
    ExplorationMappingContext->MapKey(InteractAction, EKeys::E);
    ExplorationMappingContext->MapKey(PauseAction, EKeys::Escape);
    ExplorationMappingContext->MapKey(ConflictApproach1Action, EKeys::One);
    ExplorationMappingContext->MapKey(ConflictApproach2Action, EKeys::Two);
    ExplorationMappingContext->MapKey(ConflictApproach3Action, EKeys::Three);
    ExplorationMappingContext->MapKey(ConflictApproach4Action, EKeys::Four);
    ExplorationMappingContext->MapKey(ConflictApproach5Action, EKeys::Five);
}

void ATruongSinhCharacter::BeginPlay()
{
    Super::BeginPlay();

    ActiveAnimation = IdleAnimation;
    if (ActiveAnimation)
    {
        GetMesh()->PlayAnimation(ActiveAnimation, true);
    }
}

void ATruongSinhCharacter::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    UAnimationAsset* DesiredAnimation = IdleAnimation;
    if (GetCharacterMovement()->IsFalling())
    {
        DesiredAnimation = FallAnimation;
    }
    else if (GetVelocity().SizeSquared2D() > FMath::Square(10.0f))
    {
        DesiredAnimation = JogAnimation;
    }

    if (DesiredAnimation && DesiredAnimation != ActiveAnimation)
    {
        ActiveAnimation = DesiredAnimation;
        GetMesh()->PlayAnimation(ActiveAnimation, true);
    }
}

void ATruongSinhCharacter::PawnClientRestart()
{
    Super::PawnClientRestart();

    if (const APlayerController* PlayerController = Cast<APlayerController>(Controller))
    {
        if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
            ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
        {
            Subsystem->RemoveMappingContext(ExplorationMappingContext);
            Subsystem->AddMappingContext(ExplorationMappingContext, 0);
        }
    }
}

void ATruongSinhCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    UEnhancedInputComponent* EnhancedInput = CastChecked<UEnhancedInputComponent>(PlayerInputComponent);
    EnhancedInput->BindAction(MoveForwardAction, ETriggerEvent::Triggered, this, &ATruongSinhCharacter::MoveForward);
    EnhancedInput->BindAction(MoveBackwardAction, ETriggerEvent::Triggered, this, &ATruongSinhCharacter::MoveBackward);
    EnhancedInput->BindAction(MoveLeftAction, ETriggerEvent::Triggered, this, &ATruongSinhCharacter::MoveLeft);
    EnhancedInput->BindAction(MoveRightAction, ETriggerEvent::Triggered, this, &ATruongSinhCharacter::MoveRight);
    EnhancedInput->BindAction(LookYawAction, ETriggerEvent::Triggered, this, &ATruongSinhCharacter::LookYaw);
    EnhancedInput->BindAction(LookPitchAction, ETriggerEvent::Triggered, this, &ATruongSinhCharacter::LookPitch);
    EnhancedInput->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
    EnhancedInput->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);
    EnhancedInput->BindAction(InteractAction, ETriggerEvent::Started, this, &ATruongSinhCharacter::Interact);
    EnhancedInput->BindAction(PauseAction, ETriggerEvent::Started, this, &ATruongSinhCharacter::TogglePause);
    EnhancedInput->BindAction(ConflictApproach1Action, ETriggerEvent::Started, this,
        &ATruongSinhCharacter::SelectConflictApproach1);
    EnhancedInput->BindAction(ConflictApproach2Action, ETriggerEvent::Started, this,
        &ATruongSinhCharacter::SelectConflictApproach2);
    EnhancedInput->BindAction(ConflictApproach3Action, ETriggerEvent::Started, this,
        &ATruongSinhCharacter::SelectConflictApproach3);
    EnhancedInput->BindAction(ConflictApproach4Action, ETriggerEvent::Started, this,
        &ATruongSinhCharacter::SelectConflictApproach4);
    EnhancedInput->BindAction(ConflictApproach5Action, ETriggerEvent::Started, this,
        &ATruongSinhCharacter::SelectConflictApproach5);
}

void ATruongSinhCharacter::MoveForward(const FInputActionValue& Value)
{
    if (!Controller || !Value.Get<bool>())
    {
        return;
    }

    const FRotator ControlRotation = Controller->GetControlRotation();
    const FRotator YawRotation(0.0f, ControlRotation.Yaw, 0.0f);
    AddMovementInput(FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X), 1.0f);
}

void ATruongSinhCharacter::MoveBackward(const FInputActionValue& Value)
{
    if (Value.Get<bool>())
    {
        if (Controller)
        {
            const FRotator YawRotation(0.0f, Controller->GetControlRotation().Yaw, 0.0f);
            AddMovementInput(FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X), -1.0f);
        }
    }
}

void ATruongSinhCharacter::MoveLeft(const FInputActionValue& Value)
{
    if (Controller && Value.Get<bool>())
    {
        const FRotator YawRotation(0.0f, Controller->GetControlRotation().Yaw, 0.0f);
        AddMovementInput(FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y), -1.0f);
    }
}

void ATruongSinhCharacter::MoveRight(const FInputActionValue& Value)
{
    if (!Controller || !Value.Get<bool>())
    {
        return;
    }

    const FRotator ControlRotation = Controller->GetControlRotation();
    const FRotator YawRotation(0.0f, ControlRotation.Yaw, 0.0f);
    AddMovementInput(FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y), 1.0f);
}

void ATruongSinhCharacter::LookYaw(const FInputActionValue& Value)
{
    AddControllerYawInput(Value.Get<float>());
}

void ATruongSinhCharacter::LookPitch(const FInputActionValue& Value)
{
    AddControllerPitchInput(-Value.Get<float>());
}

void ATruongSinhCharacter::Interact()
{
    if (ATruongSinhPlayerController* PlayerController = Cast<ATruongSinhPlayerController>(Controller))
    {
        PlayerController->TryInteract();
    }
}

void ATruongSinhCharacter::TogglePause()
{
    if (ATruongSinhPlayerController* PlayerController = Cast<ATruongSinhPlayerController>(Controller))
    {
        PlayerController->TogglePauseMenu();
    }
}

void ATruongSinhCharacter::SelectConflictApproach1()
{
    if (ATruongSinhPlayerController* PlayerController = Cast<ATruongSinhPlayerController>(Controller))
    {
        PlayerController->SelectConflictApproach(1);
    }
}

void ATruongSinhCharacter::SelectConflictApproach2()
{
    if (ATruongSinhPlayerController* PlayerController = Cast<ATruongSinhPlayerController>(Controller))
    {
        PlayerController->SelectConflictApproach(2);
    }
}

void ATruongSinhCharacter::SelectConflictApproach3()
{
    if (ATruongSinhPlayerController* PlayerController = Cast<ATruongSinhPlayerController>(Controller))
    {
        PlayerController->SelectConflictApproach(3);
    }
}

void ATruongSinhCharacter::SelectConflictApproach4()
{
    if (ATruongSinhPlayerController* PlayerController = Cast<ATruongSinhPlayerController>(Controller))
    {
        PlayerController->SelectConflictApproach(4);
    }
}

void ATruongSinhCharacter::SelectConflictApproach5()
{
    if (ATruongSinhPlayerController* PlayerController = Cast<ATruongSinhPlayerController>(Controller))
    {
        PlayerController->SelectConflictApproach(5);
    }
}
