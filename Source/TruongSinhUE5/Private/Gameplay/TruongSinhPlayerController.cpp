#include "Gameplay/TruongSinhPlayerController.h"

#include "Camera/CameraComponent.h"
#include "Core/TruongSinhTypes.h"
#include "Engine/Engine.h"
#include "Engine/GameViewportClient.h"
#include "Engine/World.h"
#include "GameFramework/Pawn.h"
#include "Gameplay/TruongSinhCharacter.h"
#include "HAL/FileManager.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Resolution/TruongSinhActivityResolution.h"
#include "Save/TruongSinhSaveGameV2.h"
#include "Simulation/TruongSinhGameSimulation.h"
#include "Simulation/TruongSinhLifeState.h"
#include "TruongSinhInteractionProvider.h"
#include "TruongSinhUE5.h"
#include "UI/TruongSinhRuntimeHUDWidget.h"

namespace
{
FString SavePath()
{
    return FPaths::Combine(FPaths::ProjectSavedDir(), TEXT("SaveGames"), TEXT("TruongSinh_Autosave_v2.json"));
}

FText OutcomeText(const ETruongSinhResolutionOutcome Outcome)
{
    switch (Outcome)
    {
    case ETruongSinhResolutionOutcome::GreatSuccess:
        return NSLOCTEXT("TruongSinh", "GreatSuccess", "ĐẠI THÀNH CÔNG");
    case ETruongSinhResolutionOutcome::Success:
        return NSLOCTEXT("TruongSinh", "Success", "THÀNH CÔNG");
    case ETruongSinhResolutionOutcome::PartialSuccess:
        return NSLOCTEXT("TruongSinh", "PartialSuccess", "THÀNH CÔNG MỘT PHẦN");
    case ETruongSinhResolutionOutcome::Failure:
        return NSLOCTEXT("TruongSinh", "Failure", "THẤT BẠI");
    default:
        return NSLOCTEXT("TruongSinh", "Rejected", "KHÔNG THỂ THỰC HIỆN");
    }
}
}

void ATruongSinhPlayerController::BeginPlay()
{
    Super::BeginPlay();
    ApplyGameplayMouseCapture();

    RuntimeHUD = CreateWidget<UTruongSinhRuntimeHUDWidget>(this, UTruongSinhRuntimeHUDWidget::StaticClass());
    if (RuntimeHUD)
    {
        RuntimeHUD->AddToPlayerScreen(20);
    }

    FString LoadError;
    const bool bContinued = RestoreCanonicalState(LoadError);
    if (!bContinued)
    {
        if (UTruongSinhGameSimulationFacade* Simulation =
            GetGameInstance()->GetSubsystem<UTruongSinhGameSimulationFacade>())
        {
            Simulation->StartNewGame(141210);
        }
        UE_LOG(LogTruongSinh, Log, TEXT("Starting a new deterministic sandbox: %s"), *LoadError);
    }

    if (RuntimeHUD)
    {
        RuntimeHUD->RefreshState();
        if (bContinued && !PendingReplayId.IsEmpty())
        {
            RuntimeHUD->ShowActivityResult(
                NSLOCTEXT("TruongSinh", "ContinuedResult", "KẾT QUẢ ĐÃ GHI NHẬN"),
                NSLOCTEXT("TruongSinh", "ContinuedResultDetail", "Hoạt động trước đã commit và được khôi phục; phần thưởng không áp dụng lần hai."),
                true);
            PendingReplayId.Reset();
            FString ClearReplayError;
            if (!SaveCanonicalState(ClearReplayError))
            {
                UE_LOG(LogTruongSinh, Error, TEXT("Could not clear completed replay marker: %s"), *ClearReplayError);
            }
        }
    }

    ClientMessage(TEXT("TRƯỜNG SINH · WASD di chuyển · Chuột quan sát · E tương tác · Esc tạm dừng"));
}

void ATruongSinhPlayerController::PlayerTick(const float DeltaTime)
{
    Super::PlayerTick(DeltaTime);
    if (!RuntimeHUD || IsPaused())
    {
        return;
    }

    AActor* Provider = nullptr;
    FTruongSinhInteractionOffer Offer;
    const bool bHasOffer = FindBestInteraction(Provider, Offer);
    RuntimeHUD->SetInteractionPrompt(
        bHasOffer ? NSLOCTEXT("TruongSinh", "CultivatePrompt", "Tu luyện tám canh giờ") : FText::GetEmpty(),
        bHasOffer);
}

void ATruongSinhPlayerController::ApplyGameplayMouseCapture()
{
    bShowMouseCursor = false;
    FInputModeGameOnly InputMode;
    InputMode.SetConsumeCaptureMouseDown(false);
    SetInputMode(InputMode);

    if (UGameViewportClient* Viewport = GetWorld() ? GetWorld()->GetGameViewport() : nullptr)
    {
        Viewport->SetMouseCaptureMode(EMouseCaptureMode::CapturePermanently_IncludingInitialMouseDown);
        Viewport->SetMouseLockMode(EMouseLockMode::LockAlways);
    }
}

void ATruongSinhPlayerController::TryInteract()
{
    AActor* Provider = nullptr;
    FTruongSinhInteractionOffer Offer;
    if (!FindBestInteraction(Provider, Offer))
    {
        if (RuntimeHUD)
        {
            RuntimeHUD->SetInteractionPrompt(FText::GetEmpty(), false);
        }
        ClientMessage(TEXT("Không có điểm tương tác trong tầm nhìn."));
        return;
    }

    UTruongSinhGameSimulationFacade* Simulation =
        GetGameInstance()->GetSubsystem<UTruongSinhGameSimulationFacade>();
    if (!Simulation)
    {
        ClientMessage(TEXT("Simulation facade chưa sẵn sàng."));
        return;
    }

    const FTruongSinhSimulationState Before = Simulation->GetState();
    FTruongSinhActivityPlan Plan;
    Plan.Action = ITruongSinhInteractionProvider::Execute_BuildInteractionCommand(
        Provider, Offer.CandidateId, Before.CurrentVessel.VesselId,
        Before.WorldRevision, Before.CommittedCommandIds.Num());
    Plan.Type = ETruongSinhActivityType::Cultivation;
    Plan.ActivityId.Value = TEXT("activity.cultivation.breathing_cycle");
    Plan.MethodId.Value = TEXT("method.five_elements_breathing");
    Plan.FacilityId = Offer.CandidateId;
    Plan.LocationId.Value = TEXT("zone.lower_realm.dev_smoke");
    Plan.DurationMinutes = 480;
    Plan.Strategy = ETruongSinhActivityStrategy::Balanced;

    FTruongSinhActivitySnapshot Snapshot;
    Snapshot.PerformerPower = FMath::Min<int64>(MAX_int64 - 6000, Before.CurrentVessel.CultivationUnits) + 6000;
    Snapshot.DifficultyOrTargetPower = 6500;
    Snapshot.TechniqueModifierUnits = 350 + Before.Soul.KnownTechniqueIds.Num() * 150;
    Snapshot.PreparationModifierUnits = 300;
    Snapshot.EnvironmentModifierUnits = 450;
    Snapshot.MasterSeed = Before.Rng.MasterSeed;

    const FTruongSinhActivityPreview Preview = FTruongSinhAutoResolver::Preview(Snapshot, Plan);
    if (!Preview.bEligible)
    {
        ClientMessage(TEXT("Kế hoạch tu luyện không hợp lệ."));
        return;
    }

    const FTruongSinhAutoResolutionResult Resolution = FTruongSinhAutoResolver::Resolve(Snapshot, Plan);
    FTruongSinhCultivationCommitPayload CommitPayload;
    CommitPayload.Minutes = Resolution.TimeAdvancedMinutes;
    CommitPayload.CultivationProgressUnits = Resolution.CultivationProgressUnits;
    CommitPayload.OutcomeId = Resolution.OutcomeId;
    CommitPayload.ReplayId = Resolution.ReplayId;
    Plan.Action.Payload.InitializeAs<FTruongSinhCultivationCommitPayload>(CommitPayload);
    const FTruongSinhActionResult Commit = Simulation->Execute(Plan.Action);
    if (Commit.Status != ETruongSinhActionStatus::Committed)
    {
        ClientMessage(FString::Printf(TEXT("Không thể commit: %s"), *Commit.ReasonId.Value));
        return;
    }

    PendingReplayId = Resolution.ReplayId.Value;
    FString SaveError;
    const bool bSaved = SaveCanonicalState(SaveError);
    const FString SaveStatus = bSaved ? TEXT(" · Đã lưu") : FString::Printf(TEXT(" · Lỗi lưu: %s"), *SaveError);
    const FString ResultDetails = FString::Printf(
        TEXT("Điểm %lld / %lld\nThời gian +%lld phút · Thiên đạo #%lld%s"),
        static_cast<long long>(Resolution.FinalScore),
        static_cast<long long>(Resolution.TargetScore),
        static_cast<long long>(Resolution.TimeAdvancedMinutes),
        static_cast<long long>(Commit.NewWorldRevision),
        *SaveStatus);
    if (RuntimeHUD)
    {
        RuntimeHUD->ShowActivityResult(
            OutcomeText(Resolution.Outcome),
            FText::FromString(ResultDetails),
            Resolution.Outcome != ETruongSinhResolutionOutcome::Failure &&
                Resolution.Outcome != ETruongSinhResolutionOutcome::Rejected);
    }
    ClientMessage(FString::Printf(
        TEXT("%s · Điểm %lld/%lld · +%lld phút · Revision %lld%s"),
        *OutcomeText(Resolution.Outcome).ToString(),
        static_cast<long long>(Resolution.FinalScore),
        static_cast<long long>(Resolution.TargetScore),
        static_cast<long long>(Resolution.TimeAdvancedMinutes),
        static_cast<long long>(Commit.NewWorldRevision),
        *SaveStatus));
}

bool ATruongSinhPlayerController::FindBestInteraction(
    AActor*& OutProvider,
    FTruongSinhInteractionOffer& OutOffer) const
{
    OutProvider = nullptr;
    const ATruongSinhCharacter* PawnCharacter = Cast<ATruongSinhCharacter>(GetPawn());
    const UTruongSinhGameSimulationFacade* Simulation = GetGameInstance() ?
        GetGameInstance()->GetSubsystem<UTruongSinhGameSimulationFacade>() : nullptr;
    if (!PawnCharacter || !PawnCharacter->FollowCamera || !GetWorld() || !Simulation)
    {
        return false;
    }

    const FVector Start = PawnCharacter->FollowCamera->GetComponentLocation();
    const FVector End = Start + PawnCharacter->FollowCamera->GetForwardVector() * 600.0f;
    FHitResult Hit;
    FCollisionQueryParams Query(SCENE_QUERY_STAT(TruongSinhInteraction), false, PawnCharacter);
    if (!GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, Query) ||
        !Hit.GetActor() || !Hit.GetActor()->Implements<UTruongSinhInteractionProvider>())
    {
        return false;
    }

    const FTruongSinhSimulationState State = Simulation->GetState();
    const TArray<FTruongSinhInteractionOffer> Offers =
        ITruongSinhInteractionProvider::Execute_GetInteractionOffers(Hit.GetActor(), State.CurrentVessel.VesselId);
    const float PawnDistance = FVector::Dist(PawnCharacter->GetActorLocation(), Hit.ImpactPoint);
    if (!FTruongSinhInteractionSelection::SelectBestOffer(Offers, PawnDistance, OutOffer))
    {
        return false;
    }

    OutProvider = Hit.GetActor();
    return true;
}

void ATruongSinhPlayerController::TogglePauseMenu()
{
    const bool bWillPause = !IsPaused();
    if (!SetPause(bWillPause))
    {
        UE_LOG(LogTruongSinh, Warning, TEXT("The current game mode rejected the pause state change."));
        return;
    }
    const bool bIsPaused = IsPaused();
    if (RuntimeHUD)
    {
        RuntimeHUD->SetPaused(bIsPaused);
    }
    if (bIsPaused)
    {
        bShowMouseCursor = true;
        FInputModeGameAndUI InputMode;
        InputMode.SetHideCursorDuringCapture(false);
        InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
        SetInputMode(InputMode);
        ClientMessage(TEXT("TẠM DỪNG · Nhấn Esc để tiếp tục"));
    }
    else
    {
        ApplyGameplayMouseCapture();
    }
}

bool ATruongSinhPlayerController::SaveCanonicalState(FString& OutError) const
{
    const UTruongSinhGameSimulationFacade* Simulation =
        GetGameInstance()->GetSubsystem<UTruongSinhGameSimulationFacade>();
    if (!Simulation)
    {
        OutError = TEXT("Simulation facade unavailable");
        return false;
    }

    FTruongSinhSaveGameV2 Save;
    Save.GameBuildId = TEXT("m2a-windows-native");
    Save.Simulation = Simulation->GetState();
    Save.PayloadHash = FTruongSinhGameSimulation::ComputeStateHash(Save.Simulation);
    Save.PendingReplayId.Value = PendingReplayId;

    FString Json;
    if (!FTruongSinhSaveJsonCodecV2::Serialize(Save, Json, OutError))
    {
        return false;
    }

    const FString Destination = SavePath();
    const FString Temporary = Destination + TEXT(".tmp");
    const FString Backup = Destination + TEXT(".bak");
    IFileManager::Get().MakeDirectory(*FPaths::GetPath(Destination), true);
    if (!FFileHelper::SaveStringToFile(Json, *Temporary, FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM))
    {
        OutError = TEXT("Cannot write temporary save");
        return false;
    }
    FString WrittenJson;
    FTruongSinhSaveGameV2 Verified;
    if (!FFileHelper::LoadFileToString(WrittenJson, *Temporary) ||
        !FTruongSinhSaveJsonCodecV2::Deserialize(WrittenJson, Verified, OutError))
    {
        OutError = TEXT("Temporary save verification failed: ") + OutError;
        return false;
    }
    if (IFileManager::Get().FileExists(*Destination))
    {
        if (IFileManager::Get().Copy(*Backup, *Destination, true, true) != COPY_OK)
        {
            OutError = TEXT("Cannot rotate current save to backup");
            return false;
        }
    }
    if (!IFileManager::Get().Move(*Destination, *Temporary, true, true))
    {
        OutError = TEXT("Cannot atomically promote temporary save");
        return false;
    }
    return true;
}

bool ATruongSinhPlayerController::RestoreCanonicalState(FString& OutError)
{
    UTruongSinhGameSimulationFacade* Simulation =
        GetGameInstance()->GetSubsystem<UTruongSinhGameSimulationFacade>();
    if (!Simulation)
    {
        OutError = TEXT("Simulation facade unavailable");
        return false;
    }

    const FString Destination = SavePath();
    const FString Backup = Destination + TEXT(".bak");
    for (const FString& Candidate : {Destination, Backup})
    {
        FString Json;
        FTruongSinhSaveGameV2 Save;
        if (FFileHelper::LoadFileToString(Json, *Candidate) &&
            FTruongSinhSaveJsonCodecV2::Deserialize(Json, Save, OutError))
        {
            Simulation->RestoreFromSave(Save.Simulation);
            PendingReplayId = Save.PendingReplayId.Value;
            UE_LOG(LogTruongSinh, Log, TEXT("Continued canonical state from %s"), *Candidate);
            return true;
        }
    }
    OutError = TEXT("No valid autosave or backup found");
    return false;
}
