#include "Gameplay/TruongSinhBreakthroughSite.h"

#include "Components/PointLightComponent.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/CollisionProfile.h"
#include "Simulation/TruongSinhGameSimulation.h"
#include "UObject/ConstructorHelpers.h"

namespace
{
const TCHAR* BreakthroughSiteId = TEXT("facility.breakthrough.foundation");
}

ATruongSinhBreakthroughSite::ATruongSinhBreakthroughSite()
{
    PrimaryActorTick.bCanEverTick = false;

    SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
    SetRootComponent(SceneRoot);

    DaisMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DaisMesh"));
    DaisMesh->SetupAttachment(SceneRoot);
    DaisMesh->SetRelativeScale3D(FVector(2.3f, 2.3f, 0.18f));
    DaisMesh->SetCollisionProfileName(UCollisionProfile::BlockAll_ProfileName);

    BeaconMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BeaconMesh"));
    BeaconMesh->SetupAttachment(SceneRoot);
    BeaconMesh->SetRelativeLocation(FVector(0.0f, 0.0f, 95.0f));
    BeaconMesh->SetRelativeScale3D(FVector(0.62f, 0.62f, 1.35f));
    BeaconMesh->SetCollisionProfileName(UCollisionProfile::BlockAll_ProfileName);

    static ConstructorHelpers::FObjectFinder<UStaticMesh> CylinderMesh(
        TEXT("/Engine/BasicShapes/Cylinder.Cylinder"));
    static ConstructorHelpers::FObjectFinder<UStaticMesh> ConeMesh(
        TEXT("/Engine/BasicShapes/Cone.Cone"));
    if (CylinderMesh.Succeeded())
    {
        DaisMesh->SetStaticMesh(CylinderMesh.Object);
    }
    if (ConeMesh.Succeeded())
    {
        BeaconMesh->SetStaticMesh(ConeMesh.Object);
    }

    BeaconLight = CreateDefaultSubobject<UPointLightComponent>(TEXT("BeaconLight"));
    BeaconLight->SetupAttachment(SceneRoot);
    BeaconLight->SetRelativeLocation(FVector(0.0f, 0.0f, 160.0f));
    BeaconLight->SetIntensity(1150.0f);
    BeaconLight->SetAttenuationRadius(520.0f);
    BeaconLight->SetLightColor(FLinearColor(1.0f, 0.56f, 0.18f));
    BeaconLight->SetCastShadows(true);

    SetActorEnableCollision(true);
}

TArray<FTruongSinhInteractionOffer> ATruongSinhBreakthroughSite::GetInteractionOffers_Implementation(
    const FTruongSinhStableId InstigatorId) const
{
    FTruongSinhInteractionOffer Offer;
    Offer.CandidateId.Value = BreakthroughSiteId;
    Offer.PromptStringKey = TEXT("interaction.breakthrough.foundation");
    Offer.Priority = 110;
    Offer.MaximumRangeCentimeters = 225.0f;
    Offer.bEnabled = InstigatorId.IsValid();
    if (!Offer.bEnabled)
    {
        Offer.DisabledReasonId.Value = TEXT("interaction.invalid_instigator");
    }
    return {Offer};
}

FTruongSinhActionCommand ATruongSinhBreakthroughSite::BuildInteractionCommand_Implementation(
    const FTruongSinhStableId CandidateId,
    const FTruongSinhStableId InstigatorId,
    const int64 ExpectedWorldRevision,
    const int64 Sequence) const
{
    FTruongSinhActionCommand Command;
    Command.CommandId = FGuid(
        0x54534252u,
        static_cast<uint32>(ExpectedWorldRevision),
        static_cast<uint32>(ExpectedWorldRevision >> 32),
        static_cast<uint32>(Sequence));
    Command.ActionId.Value = FTruongSinhGameSimulation::CommitResolvedActivityActionId;
    Command.InstigatorId = InstigatorId;
    Command.TargetIds.Add(CandidateId);
    Command.ExpectedWorldRevision = ExpectedWorldRevision;
    Command.Sequence = Sequence;
    return Command;
}
