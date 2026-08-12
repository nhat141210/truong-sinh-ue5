#include "Gameplay/TruongSinhAlchemySite.h"

#include "Components/PointLightComponent.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/CollisionProfile.h"
#include "Simulation/TruongSinhGameSimulation.h"
#include "UObject/ConstructorHelpers.h"

namespace { const TCHAR* AlchemySiteId = TEXT("facility.alchemy.qingxin"); }

ATruongSinhAlchemySite::ATruongSinhAlchemySite()
{
    PrimaryActorTick.bCanEverTick = false;
    SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot")); SetRootComponent(SceneRoot);
    FurnaceMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FurnaceMesh")); FurnaceMesh->SetupAttachment(SceneRoot);
    FurnaceMesh->SetRelativeScale3D(FVector(1.25f, 1.25f, 1.6f)); FurnaceMesh->SetCollisionProfileName(UCollisionProfile::BlockAll_ProfileName);
    FlameMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FlameMesh")); FlameMesh->SetupAttachment(SceneRoot);
    FlameMesh->SetRelativeLocation(FVector(0.0f, 0.0f, 150.0f)); FlameMesh->SetRelativeScale3D(FVector(0.55f)); FlameMesh->SetCollisionProfileName(UCollisionProfile::BlockAll_ProfileName);
    static ConstructorHelpers::FObjectFinder<UStaticMesh> Cylinder(TEXT("/Engine/BasicShapes/Cylinder.Cylinder"));
    static ConstructorHelpers::FObjectFinder<UStaticMesh> Sphere(TEXT("/Engine/BasicShapes/Sphere.Sphere"));
    if (Cylinder.Succeeded()) FurnaceMesh->SetStaticMesh(Cylinder.Object);
    if (Sphere.Succeeded()) FlameMesh->SetStaticMesh(Sphere.Object);
    FurnaceLight = CreateDefaultSubobject<UPointLightComponent>(TEXT("FurnaceLight")); FurnaceLight->SetupAttachment(SceneRoot);
    FurnaceLight->SetRelativeLocation(FVector(0.0f, 0.0f, 165.0f)); FurnaceLight->SetIntensity(1000.0f);
    FurnaceLight->SetAttenuationRadius(440.0f); FurnaceLight->SetLightColor(FLinearColor(1.0f, 0.22f, 0.05f)); FurnaceLight->SetCastShadows(true);
    SetActorEnableCollision(true);
}

TArray<FTruongSinhInteractionOffer> ATruongSinhAlchemySite::GetInteractionOffers_Implementation(FTruongSinhStableId InstigatorId) const
{
    FTruongSinhInteractionOffer Offer; Offer.CandidateId.Value = AlchemySiteId;
    Offer.PromptStringKey = TEXT("interaction.alchemy.qingxin"); Offer.Priority = 105;
    Offer.MaximumRangeCentimeters = 225.0f; Offer.bEnabled = InstigatorId.IsValid();
    if (!Offer.bEnabled) Offer.DisabledReasonId.Value = TEXT("interaction.invalid_instigator");
    return {Offer};
}

FTruongSinhActionCommand ATruongSinhAlchemySite::BuildInteractionCommand_Implementation(
    FTruongSinhStableId CandidateId, FTruongSinhStableId InstigatorId, int64 ExpectedWorldRevision, int64 Sequence) const
{
    FTruongSinhActionCommand Command; Command.CommandId = FGuid(0x5453414Cu, static_cast<uint32>(ExpectedWorldRevision),
        static_cast<uint32>(ExpectedWorldRevision >> 32), static_cast<uint32>(Sequence));
    Command.ActionId.Value = FTruongSinhGameSimulation::CommitResolvedActivityActionId;
    Command.InstigatorId = InstigatorId; Command.TargetIds.Add(CandidateId);
    Command.ExpectedWorldRevision = ExpectedWorldRevision; Command.Sequence = Sequence; return Command;
}
