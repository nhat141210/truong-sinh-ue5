#include "Gameplay/TruongSinhCultivationSite.h"

#include "Components/PointLightComponent.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/CollisionProfile.h"
#include "Simulation/TruongSinhGameSimulation.h"
#include "UObject/ConstructorHelpers.h"

namespace
{
const TCHAR* CultivationSiteId = TEXT("facility.cultivation.dev_smoke");
constexpr int64 CultivationDurationMinutes = 480;
}

ATruongSinhCultivationSite::ATruongSinhCultivationSite()
{
    PrimaryActorTick.bCanEverTick = false;

    SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
    SetRootComponent(SceneRoot);

    AltarMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("AltarMesh"));
    AltarMesh->SetupAttachment(SceneRoot);
    AltarMesh->SetRelativeScale3D(FVector(1.8f, 1.8f, 0.28f));
    AltarMesh->SetCollisionProfileName(UCollisionProfile::BlockAll_ProfileName);

    FocusMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FocusMesh"));
    FocusMesh->SetupAttachment(SceneRoot);
    FocusMesh->SetRelativeLocation(FVector(0.0f, 0.0f, 82.0f));
    FocusMesh->SetRelativeScale3D(FVector(0.42f));
    FocusMesh->SetCollisionProfileName(UCollisionProfile::BlockAll_ProfileName);

    static ConstructorHelpers::FObjectFinder<UStaticMesh> CylinderMesh(
        TEXT("/Engine/BasicShapes/Cylinder.Cylinder"));
    static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereMesh(
        TEXT("/Engine/BasicShapes/Sphere.Sphere"));
    if (CylinderMesh.Succeeded())
    {
        AltarMesh->SetStaticMesh(CylinderMesh.Object);
    }
    if (SphereMesh.Succeeded())
    {
        FocusMesh->SetStaticMesh(SphereMesh.Object);
    }

    FocusLight = CreateDefaultSubobject<UPointLightComponent>(TEXT("FocusLight"));
    FocusLight->SetupAttachment(SceneRoot);
    FocusLight->SetRelativeLocation(FVector(0.0f, 0.0f, 115.0f));
    FocusLight->SetIntensity(900.0f);
    FocusLight->SetAttenuationRadius(480.0f);
    FocusLight->SetLightColor(FLinearColor(0.34f, 0.68f, 1.0f));
    FocusLight->SetCastShadows(true);

    SetActorEnableCollision(true);
}

TArray<FTruongSinhInteractionOffer> ATruongSinhCultivationSite::GetInteractionOffers_Implementation(
    const FTruongSinhStableId InstigatorId) const
{
    FTruongSinhInteractionOffer Offer;
    Offer.CandidateId.Value = CultivationSiteId;
    Offer.PromptStringKey = TEXT("interaction.cultivate.eight_hours");
    Offer.Priority = 100;
    Offer.MaximumRangeCentimeters = 225.0f;
    Offer.bEnabled = InstigatorId.IsValid();
    if (!Offer.bEnabled)
    {
        Offer.DisabledReasonId.Value = TEXT("interaction.invalid_instigator");
    }
    return {Offer};
}

FTruongSinhActionCommand ATruongSinhCultivationSite::BuildInteractionCommand_Implementation(
    const FTruongSinhStableId CandidateId,
    const FTruongSinhStableId InstigatorId,
    const int64 ExpectedWorldRevision,
    const int64 Sequence) const
{
    FTruongSinhActionCommand Command;
    Command.CommandId = FGuid(
        0x54534355u,
        static_cast<uint32>(ExpectedWorldRevision),
        static_cast<uint32>(ExpectedWorldRevision >> 32),
        static_cast<uint32>(Sequence));
    Command.ActionId.Value = FTruongSinhGameSimulation::CommitCultivationActionId;
    Command.InstigatorId = InstigatorId;
    Command.TargetIds.Add(CandidateId);
    Command.ExpectedWorldRevision = ExpectedWorldRevision;
    Command.Sequence = Sequence;

    return Command;
}
