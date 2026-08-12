#include "Gameplay/TruongSinhPlayerController.h"

#include "Camera/CameraComponent.h"
#include "Core/TruongSinhTypes.h"
#include "Data/TruongSinhActivityRegistryDataAsset.h"
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
#include "UObject/ConstructorHelpers.h"

namespace
{
FString SavePath()
{
    return FPaths::Combine(FPaths::ProjectSavedDir(), TEXT("SaveGames"), TEXT("TruongSinh_Autosave_v2.json"));
}

bool IsBreakthroughDefinition(const FTruongSinhActivityDefinition& Definition)
{
    return Definition.ResolverId == TEXT("breakthrough");
}

bool TryGetActivityType(const FTruongSinhActivityDefinition& Definition, ETruongSinhActivityType& OutType)
{
    if (Definition.ResolverId == TEXT("cultivation"))
    {
        OutType = ETruongSinhActivityType::Cultivation;
        return true;
    }
    if (Definition.ResolverId == TEXT("breakthrough"))
    {
        OutType = ETruongSinhActivityType::Breakthrough;
        return true;
    }
    if (Definition.ResolverId == TEXT("alchemy"))
    {
        OutType = ETruongSinhActivityType::Alchemy;
        return true;
    }
    return false;
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

ATruongSinhPlayerController::ATruongSinhPlayerController(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    static ConstructorHelpers::FObjectFinder<UTruongSinhActivityRegistryDataAsset> RegistryAsset(
        TEXT("/Game/Data/DA_ActivityRegistry.DA_ActivityRegistry"));
    ActivityRegistry = RegistryAsset.Object;
}

void ATruongSinhPlayerController::BeginPlay()
{
    Super::BeginPlay();
    ApplyGameplayMouseCapture();

    if (!ActivityRegistry)
    {
        UE_LOG(LogTruongSinh, Error, TEXT("Shared activity registry is unavailable; interactions are disabled."));
    }
    else
    {
        FString RegistryError;
        if (!ActivityRegistry->ValidateRegistry(RegistryError))
        {
            UE_LOG(LogTruongSinh, Error, TEXT("Shared activity registry is invalid: %s"), *RegistryError);
        }
        else
        {
            UE_LOG(LogTruongSinh, Log, TEXT("Shared activity registry loaded: %d definitions."),
                ActivityRegistry->Definitions.Num());
        }
    }

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
    const FTruongSinhActivityDefinition* Definition = bHasOffer && ActivityRegistry ?
        ActivityRegistry->FindByFacility(Offer.CandidateId) : nullptr;
    const bool bHasRegisteredOffer = Definition != nullptr;
    RuntimeHUD->SetInteractionPrompt(
        bHasRegisteredOffer ? (IsBreakthroughDefinition(*Definition)
            ? NSLOCTEXT("TruongSinh", "BreakthroughPrompt", "Đột phá Trúc Cơ")
            : Definition->ResolverId == TEXT("alchemy")
                ? NSLOCTEXT("TruongSinh", "AlchemyPrompt", "Luyện đan Thanh Tâm")
            : NSLOCTEXT("TruongSinh", "CultivatePrompt", "Tu luyện tám canh giờ")) : FText::GetEmpty(),
        bHasRegisteredOffer);
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
    if (!ActivityRegistry)
    {
        ClientMessage(TEXT("Activity registry chưa được nạp."));
        return;
    }
    FString RegistryError;
    if (!ActivityRegistry->ValidateRegistry(RegistryError))
    {
        UE_LOG(LogTruongSinh, Error, TEXT("Invalid activity registry: %s"), *RegistryError);
        ClientMessage(TEXT("Activity registry không hợp lệ."));
        return;
    }
    const FTruongSinhActivityDefinition* Definition = ActivityRegistry->FindByFacility(Offer.CandidateId);
    if (!Definition)
    {
        ClientMessage(TEXT("Điểm tương tác chưa có activity definition."));
        return;
    }
    ETruongSinhActivityType ActivityType;
    if (!TryGetActivityType(*Definition, ActivityType))
    {
        ClientMessage(TEXT("Resolver activity chưa được đăng ký."));
        return;
    }
    const bool bBreakthrough = IsBreakthroughDefinition(*Definition);
    if (bBreakthrough && Before.CurrentVessel.RealmId.Value != TEXT("realm.mortal"))
    {
        const FText Requirement = NSLOCTEXT(
            "TruongSinh", "BreakthroughAlreadyCompleted", "Bạn đã vượt qua Trúc Cơ; cảnh giới kế tiếp chưa được mở trong bản này.");
        if (RuntimeHUD)
        {
            RuntimeHUD->ShowActivityResult(
                NSLOCTEXT("TruongSinh", "BreakthroughComplete", "ĐÃ VƯỢT QUA CẢNH GIỚI"), Requirement, false);
        }
        ClientMessage(Requirement.ToString());
        return;
    }
    if (bBreakthrough && Before.CurrentVessel.CultivationUnits < 800)
    {
        const FText Requirement = NSLOCTEXT(
            "TruongSinh", "BreakthroughRequirement", "Cần ít nhất 800 tu vi trước khi đột phá Trúc Cơ.");
        if (RuntimeHUD)
        {
            RuntimeHUD->ShowActivityResult(
                NSLOCTEXT("TruongSinh", "BreakthroughUnavailable", "CẢNH GIỚI CHƯA ĐỦ"), Requirement, false);
        }
        ClientMessage(Requirement.ToString());
        return;
    }
    if (Before.CurrentVessel.CultivationUnits < Definition->MinimumCultivationUnits)
    {
        ClientMessage(FString::Printf(TEXT("Cần ít nhất %lld tu vi cho hoạt động này."),
            static_cast<long long>(Definition->MinimumCultivationUnits)));
        return;
    }

    FTruongSinhActivityPlan Plan;
    Plan.Action = ITruongSinhInteractionProvider::Execute_BuildInteractionCommand(
        Provider, Offer.CandidateId, Before.CurrentVessel.VesselId,
        Before.WorldRevision, Before.CommittedCommandIds.Num());
    Plan.Type = ActivityType;
    Plan.ActivityId = Definition->ActivityId;
    Plan.MethodId = Definition->MethodId;
    Plan.FacilityId = Definition->FacilityId;
    Plan.LocationId = Definition->LocationId;
    Plan.DurationMinutes = Definition->DurationMinutes;
    Plan.Strategy = ETruongSinhActivityStrategy::Balanced;
    Plan.OutputId = Definition->OutputId;
    Plan.MaximumOutputUnits = Definition->MaximumOutputUnits;

    FTruongSinhActivitySnapshot Snapshot;
    Snapshot.PerformerPower = FMath::Min<int64>(MAX_int64 - 6000, Before.CurrentVessel.CultivationUnits) + 6000;
    Snapshot.DifficultyOrTargetPower = Definition->DifficultyOrTargetPower;
    Snapshot.TechniqueModifierUnits = Definition->TechniqueModifierUnits + Before.Soul.KnownTechniqueIds.Num() * 150;
    Snapshot.PreparationModifierUnits = Definition->PreparationModifierUnits;
    Snapshot.EnvironmentModifierUnits = Definition->EnvironmentModifierUnits;
    Snapshot.MasterSeed = Before.Rng.MasterSeed;

    const FTruongSinhActivityPreview Preview = FTruongSinhAutoResolver::Preview(Snapshot, Plan);
    if (!Preview.bEligible)
    {
        ClientMessage(TEXT("Kế hoạch tu luyện không hợp lệ."));
        return;
    }

    const FTruongSinhAutoResolutionResult Resolution = FTruongSinhAutoResolver::Resolve(Snapshot, Plan);
    if (ActivityType != ETruongSinhActivityType::Cultivation)
    {
        FTruongSinhResolvedActivityCommitPayload CommitPayload;
        CommitPayload.ActivityId = Plan.ActivityId;
        CommitPayload.RequiredCurrentRealmId = Before.CurrentVessel.RealmId;
        CommitPayload.Minutes = Resolution.TimeAdvancedMinutes;
        CommitPayload.CultivationProgressUnits = Resolution.CultivationProgressUnits;
        CommitPayload.RealmLifespanBonusDays = Resolution.RealmLifespanBonusDays;
        CommitPayload.NewRealmId = Resolution.NewRealmId;
        CommitPayload.OutcomeId = Resolution.OutcomeId;
        CommitPayload.ReplayId = Resolution.ReplayId;
        CommitPayload.OutputId = Resolution.OutputId;
        CommitPayload.OutputUnits = Resolution.OutputUnits;
        CommitPayload.OutputQualityBps = Resolution.OutputQualityBps;
        CommitPayload.OutputImpurityBps = Resolution.OutputImpurityBps;
        Plan.Action.Payload.InitializeAs<FTruongSinhResolvedActivityCommitPayload>(CommitPayload);
    }
    else
    {
        FTruongSinhCultivationCommitPayload CommitPayload;
        CommitPayload.Minutes = Resolution.TimeAdvancedMinutes;
        CommitPayload.CultivationProgressUnits = Resolution.CultivationProgressUnits;
        CommitPayload.OutcomeId = Resolution.OutcomeId;
        CommitPayload.ReplayId = Resolution.ReplayId;
        Plan.Action.Payload.InitializeAs<FTruongSinhCultivationCommitPayload>(CommitPayload);
    }
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
    const FString ResultDetails = bBreakthrough ? FString::Printf(
        TEXT("Điểm %lld / %lld\nThọ nguyên +%lld ngày · Thời gian +%lld phút · Thiên đạo #%lld%s"),
        static_cast<long long>(Resolution.FinalScore),
        static_cast<long long>(Resolution.TargetScore),
        static_cast<long long>(Resolution.RealmLifespanBonusDays),
        static_cast<long long>(Resolution.TimeAdvancedMinutes),
        static_cast<long long>(Commit.NewWorldRevision),
        *SaveStatus) : ActivityType == ETruongSinhActivityType::Alchemy ? FString::Printf(
        TEXT("%s ×%lld\nPhẩm chất %d%% · Tạp chất %d%% · Thời gian +%lld phút%s"),
        *Resolution.OutputId.Value,
        static_cast<long long>(Resolution.OutputUnits),
        Resolution.OutputQualityBps / 100,
        Resolution.OutputImpurityBps / 100,
        static_cast<long long>(Resolution.TimeAdvancedMinutes),
        *SaveStatus) : FString::Printf(
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
