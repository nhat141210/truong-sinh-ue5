#include "Gameplay/TruongSinhFormationSite.h"

#include "Components/PointLightComponent.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/CollisionProfile.h"
#include "Simulation/TruongSinhGameSimulation.h"
#include "UObject/ConstructorHelpers.h"

namespace { const TCHAR* FormationSiteId = TEXT("facility.formation.spirit_gathering"); }

ATruongSinhFormationSite::ATruongSinhFormationSite()
{
    PrimaryActorTick.bCanEverTick = false;
    SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
    SetRootComponent(SceneRoot);
    static ConstructorHelpers::FObjectFinder<UStaticMesh> Cylinder(TEXT("/Engine/BasicShapes/Cylinder.Cylinder"));
    static ConstructorHelpers::FObjectFinder<UStaticMesh> Cone(TEXT("/Engine/BasicShapes/Cone.Cone"));

    ArrayPlate = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ArrayPlate"));
    ArrayPlate->SetupAttachment(SceneRoot); ArrayPlate->SetRelativeScale3D(FVector(2.6f, 2.6f, 0.08f));
    ArrayPlate->SetCollisionProfileName(UCollisionProfile::BlockAll_ProfileName);
    if (Cylinder.Succeeded()) ArrayPlate->SetStaticMesh(Cylinder.Object);

    UStaticMeshComponent* Anchors[] = {
        AnchorA = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("AnchorA")),
        AnchorB = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("AnchorB")),
        AnchorC = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("AnchorC"))};
    const FVector Locations[] = {FVector(105, 0, 65), FVector(-52, 91, 65), FVector(-52, -91, 65)};
    for (int32 Index = 0; Index < 3; ++Index)
    {
        Anchors[Index]->SetupAttachment(SceneRoot); Anchors[Index]->SetRelativeLocation(Locations[Index]);
        Anchors[Index]->SetRelativeScale3D(FVector(0.28f, 0.28f, 0.8f));
        Anchors[Index]->SetCollisionProfileName(UCollisionProfile::BlockAll_ProfileName);
        if (Cone.Succeeded()) Anchors[Index]->SetStaticMesh(Cone.Object);
    }

    ArrayLight = CreateDefaultSubobject<UPointLightComponent>(TEXT("ArrayLight"));
    ArrayLight->SetupAttachment(SceneRoot); ArrayLight->SetRelativeLocation(FVector(0, 0, 100));
    ArrayLight->SetIntensity(850.0f); ArrayLight->SetAttenuationRadius(500.0f);
    ArrayLight->SetLightColor(FLinearColor(0.12f, 0.9f, 0.58f)); ArrayLight->SetCastShadows(false);
    SetActorEnableCollision(true);
}

TArray<FTruongSinhInteractionOffer> ATruongSinhFormationSite::GetInteractionOffers_Implementation(FTruongSinhStableId InstigatorId) const
{
    FTruongSinhInteractionOffer Offer; Offer.CandidateId.Value = FormationSiteId;
    Offer.PromptStringKey = TEXT("interaction.formation.spirit_gathering"); Offer.Priority = 104;
    Offer.MaximumRangeCentimeters = 225.0f; Offer.bEnabled = InstigatorId.IsValid();
    if (!Offer.bEnabled) Offer.DisabledReasonId.Value = TEXT("interaction.invalid_instigator");
    return {Offer};
}

FTruongSinhActionCommand ATruongSinhFormationSite::BuildInteractionCommand_Implementation(
    FTruongSinhStableId CandidateId, FTruongSinhStableId InstigatorId, int64 ExpectedWorldRevision, int64 Sequence) const
{
    FTruongSinhActionCommand Command; Command.CommandId = FGuid(0x5453464Du,
        static_cast<uint32>(ExpectedWorldRevision), static_cast<uint32>(ExpectedWorldRevision >> 32),
        static_cast<uint32>(Sequence)); Command.ActionId.Value = FTruongSinhGameSimulation::CommitResolvedActivityActionId;
    Command.InstigatorId = InstigatorId; Command.TargetIds.Add(CandidateId);
    Command.ExpectedWorldRevision = ExpectedWorldRevision; Command.Sequence = Sequence; return Command;
}
