#include "Gameplay/TruongSinhConflictSite.h"

#include "Components/PointLightComponent.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/CollisionProfile.h"
#include "Simulation/TruongSinhGameSimulation.h"
#include "UObject/ConstructorHelpers.h"

namespace { const TCHAR* ConflictSiteId = TEXT("facility.conflict.cloud_palm_trial"); }

ATruongSinhConflictSite::ATruongSinhConflictSite()
{
    PrimaryActorTick.bCanEverTick = false;
    SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
    SetRootComponent(SceneRoot);
    static ConstructorHelpers::FObjectFinder<UStaticMesh> Cylinder(TEXT("/Engine/BasicShapes/Cylinder.Cylinder"));
    static ConstructorHelpers::FObjectFinder<UStaticMesh> Sphere(TEXT("/Engine/BasicShapes/Sphere.Sphere"));

    TrialStone = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TrialStone"));
    TrialStone->SetupAttachment(SceneRoot);
    TrialStone->SetRelativeScale3D(FVector(1.1f, 1.1f, 0.18f));
    TrialStone->SetCollisionProfileName(UCollisionProfile::BlockAll_ProfileName);
    if (Cylinder.Succeeded()) TrialStone->SetStaticMesh(Cylinder.Object);

    OpponentMarker = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("OpponentMarker"));
    OpponentMarker->SetupAttachment(SceneRoot);
    OpponentMarker->SetRelativeLocation(FVector(0, 0, 105));
    OpponentMarker->SetRelativeScale3D(FVector(0.42f, 0.42f, 1.25f));
    OpponentMarker->SetCollisionProfileName(UCollisionProfile::BlockAll_ProfileName);
    if (Sphere.Succeeded()) OpponentMarker->SetStaticMesh(Sphere.Object);

    TrialLight = CreateDefaultSubobject<UPointLightComponent>(TEXT("TrialLight"));
    TrialLight->SetupAttachment(SceneRoot);
    TrialLight->SetRelativeLocation(FVector(0, 0, 150));
    TrialLight->SetIntensity(650.0f);
    TrialLight->SetAttenuationRadius(420.0f);
    TrialLight->SetLightColor(FLinearColor(0.72f, 0.18f, 0.12f));
    TrialLight->SetCastShadows(true);
    SetActorEnableCollision(true);
}

TArray<FTruongSinhInteractionOffer> ATruongSinhConflictSite::GetInteractionOffers_Implementation(FTruongSinhStableId InstigatorId) const
{
    FTruongSinhInteractionOffer Offer;
    Offer.CandidateId.Value = ConflictSiteId;
    Offer.PromptStringKey = TEXT("interaction.conflict.cloud_palm_trial");
    Offer.Priority = 103;
    Offer.MaximumRangeCentimeters = 225.0f;
    Offer.bEnabled = InstigatorId.IsValid();
    if (!Offer.bEnabled) Offer.DisabledReasonId.Value = TEXT("interaction.invalid_instigator");
    return {Offer};
}

FTruongSinhActionCommand ATruongSinhConflictSite::BuildInteractionCommand_Implementation(
    FTruongSinhStableId CandidateId, FTruongSinhStableId InstigatorId,
    int64 ExpectedWorldRevision, int64 Sequence) const
{
    FTruongSinhActionCommand Command;
    Command.CommandId = FGuid(0x54534346u, static_cast<uint32>(ExpectedWorldRevision),
        static_cast<uint32>(ExpectedWorldRevision >> 32), static_cast<uint32>(Sequence));
    Command.ActionId.Value = FTruongSinhGameSimulation::CommitResolvedActivityActionId;
    Command.InstigatorId = InstigatorId;
    Command.TargetIds.Add(CandidateId);
    Command.ExpectedWorldRevision = ExpectedWorldRevision;
    Command.Sequence = Sequence;
    return Command;
}
