#pragma once

#include "CoreMinimal.h"
#include "Core/TruongSinhTypes.h"
#include "TruongSinhLifeState.generated.h"

USTRUCT(BlueprintType)
struct TRUONGSINHSIMULATION_API FTruongSinhLifespanState
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Truong Sinh|Life")
    int64 BiologicalAgeDays = 0;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Truong Sinh|Life")
    int64 BaseLifespanDays = 0;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Truong Sinh|Life")
    int64 RealmBonusDays = 0;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Truong Sinh|Life")
    int64 TechniqueBonusDays = 0;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Truong Sinh|Life")
    int64 PillAndResourceBonusDays = 0;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Truong Sinh|Life")
    int64 PermanentDamageDays = 0;

    int64 EffectiveLifespanDays() const;
    int64 RemainingLifespanDays() const;
    bool IsExpired() const;
};

/** Persistent player identity across bodies. Learned knowledge belongs here. */
USTRUCT(BlueprintType)
struct TRUONGSINHSIMULATION_API FTruongSinhSoulState
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Truong Sinh|Soul")
    FTruongSinhStableId SoulId;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Truong Sinh|Soul", meta = (ClampMin = "0", ClampMax = "10000"))
    int32 IntegrityUnits = 10000;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Truong Sinh|Soul")
    TArray<FTruongSinhStableId> KnownTechniqueIds;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Truong Sinh|Soul")
    TArray<FTruongSinhStableId> MemoryIds;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Truong Sinh|Soul")
    TArray<FTruongSinhStableId> VesselHistoryIds;
};

/** Body, social identity and property currently occupied by a soul. */
USTRUCT(BlueprintType)
struct TRUONGSINHSIMULATION_API FTruongSinhVesselState
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Truong Sinh|Vessel")
    FTruongSinhStableId VesselId;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Truong Sinh|Vessel")
    FTruongSinhStableId IdentityId;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Truong Sinh|Vessel")
    FTruongSinhStableId SpiritualRootId;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Truong Sinh|Vessel")
    FTruongSinhStableId RealmId;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Truong Sinh|Vessel")
    int64 CultivationUnits = 0;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Truong Sinh|Vessel")
    FTruongSinhLifespanState Lifespan;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Truong Sinh|Vessel")
    FTruongSinhStableId SectId;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Truong Sinh|Vessel")
    TArray<FTruongSinhStableId> RelationshipIds;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Truong Sinh|Vessel")
    TArray<FTruongSinhStableId> OwnedAssetIds;
};

USTRUCT(BlueprintType)
struct TRUONGSINHSIMULATION_API FTruongSinhPossessionRequest
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Truong Sinh|Soul")
    FGuid CommandId;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Truong Sinh|Soul")
    FTruongSinhStableId PossessionTechniqueId;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Truong Sinh|Soul")
    FTruongSinhStableId MediumId;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Truong Sinh|Soul", meta = (ClampMin = "0", ClampMax = "10000"))
    int32 CompatibilityBps = 0;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Truong Sinh|Soul")
    bool bCurrentBodyUnavailable = false;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Truong Sinh|Soul")
    int64 MasterSeed = 0;
};

UENUM(BlueprintType)
enum class ETruongSinhPossessionOutcome : uint8
{
    Rejected,
    Failed,
    Succeeded,
    EmergencyVessel
};

USTRUCT(BlueprintType)
struct TRUONGSINHSIMULATION_API FTruongSinhPossessionResult
{
    GENERATED_BODY()

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Truong Sinh|Soul")
    ETruongSinhPossessionOutcome Outcome = ETruongSinhPossessionOutcome::Rejected;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Truong Sinh|Soul")
    FTruongSinhStableId ReasonId;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Truong Sinh|Soul")
    FTruongSinhSoulState Soul;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Truong Sinh|Soul")
    FTruongSinhVesselState Vessel;
};

class TRUONGSINHSIMULATION_API FTruongSinhLifeRules
{
public:
    static FTruongSinhSoulState CreateInitialSoul();
    static FTruongSinhVesselState CreateInitialVessel();

    /** Target identity/assets/relations are inherited; body cultivation is reset. */
    static FTruongSinhPossessionResult ResolvePossession(
        const FTruongSinhSoulState& Soul,
        const FTruongSinhVesselState& CurrentVessel,
        const FTruongSinhVesselState& TargetVessel,
        const FTruongSinhPossessionRequest& Request);
};
