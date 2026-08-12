#pragma once

#include "CoreMinimal.h"
#include "Core/TruongSinhTypes.h"
#include "Simulation/TruongSinhLifeState.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "TruongSinhGameSimulation.generated.h"

/**
 * Canonical time payload. Exploration adapters submit fixed quanta; activities
 * submit their resolved duration. Simulation never reads per-frame timing.
 */
USTRUCT(BlueprintType)
struct TRUONGSINHSIMULATION_API FTruongSinhAdvanceTimePayload : public FTruongSinhActionPayload
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Truong Sinh|Simulation", meta = (ClampMin = "1"))
    int64 Minutes = 1;
};

/** Resolved cultivation delta. Resolution authors it; simulation validates and commits it atomically. */
USTRUCT(BlueprintType)
struct TRUONGSINHSIMULATION_API FTruongSinhCultivationCommitPayload : public FTruongSinhActionPayload
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Truong Sinh|Simulation", meta = (ClampMin = "1"))
    int64 Minutes = 1;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Truong Sinh|Simulation", meta = (ClampMin = "0"))
    int64 CultivationProgressUnits = 0;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Truong Sinh|Simulation")
    FTruongSinhStableId OutcomeId;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Truong Sinh|Simulation")
    FTruongSinhStableId ReplayId;
};

/**
 * Generic resolved-activity delta. Resolution owns all outcome values; this
 * payload is the narrow, typed hand-off that simulation validates and commits.
 * It deliberately does not reference UWorld, actors, UI, or resolver types.
 */
USTRUCT(BlueprintType)
struct TRUONGSINHSIMULATION_API FTruongSinhResolvedActivityCommitPayload : public FTruongSinhActionPayload
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Truong Sinh|Simulation")
    FTruongSinhStableId ActivityId;

    /** Canonical state precondition; prevents a resolved activity from being applied to a different realm. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Truong Sinh|Simulation")
    FTruongSinhStableId RequiredCurrentRealmId;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Truong Sinh|Simulation", meta = (ClampMin = "1"))
    int64 Minutes = 1;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Truong Sinh|Simulation", meta = (ClampMin = "0"))
    int64 CultivationProgressUnits = 0;

    /** Applied to the realm contribution only after a successful resolved breakthrough. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Truong Sinh|Simulation", meta = (ClampMin = "0"))
    int64 RealmLifespanBonusDays = 0;

    /** Empty means the vessel keeps its current realm. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Truong Sinh|Simulation")
    FTruongSinhStableId NewRealmId;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Truong Sinh|Simulation")
    FTruongSinhStableId OutcomeId;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Truong Sinh|Simulation")
    FTruongSinhStableId ReplayId;

    /** Optional resolved item output. A positive quantity requires a valid output ID. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Truong Sinh|Simulation")
    FTruongSinhStableId OutputId;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Truong Sinh|Simulation", meta = (ClampMin = "0"))
    int64 OutputUnits = 0;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Truong Sinh|Simulation", meta = (ClampMin = "0", ClampMax = "10000"))
    int32 OutputQualityBps = 0;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Truong Sinh|Simulation", meta = (ClampMin = "0", ClampMax = "10000"))
    int32 OutputImpurityBps = 0;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Truong Sinh|Simulation")
    FTruongSinhStableId FormationEffectId;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Truong Sinh|Simulation", meta = (ClampMin = "0", ClampMax = "10000"))
    int32 FormationIntegrityBps = 0;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Truong Sinh|Simulation", meta = (ClampMin = "0"))
    int64 FormationDurationMinutes = 0;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Truong Sinh|Simulation")
    FTruongSinhStableId ConflictOpponentId;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Truong Sinh|Simulation", meta = (ClampMin = "0"))
    int64 ConflictPermanentDamageDays = 0;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Truong Sinh|Simulation")
    bool bConflictOpponentDefeated = false;
};

/** Persistent result of a committed recipe/activity output. Ordered by command GUID for save/hash determinism. */
USTRUCT(BlueprintType)
struct TRUONGSINHSIMULATION_API FTruongSinhActivityOutputRecord
{
    GENERATED_BODY()

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Truong Sinh|Simulation")
    FGuid CommandId;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Truong Sinh|Simulation")
    FTruongSinhStableId ActivityId;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Truong Sinh|Simulation")
    FTruongSinhStableId OutputId;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Truong Sinh|Simulation")
    int64 Units = 0;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Truong Sinh|Simulation")
    int32 QualityBps = 0;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Truong Sinh|Simulation")
    int32 ImpurityBps = 0;
};

/** Canonical installed formation. Expiration is expressed in game time, never wall-clock time. */
USTRUCT(BlueprintType)
struct TRUONGSINHSIMULATION_API FTruongSinhFormationState
{
    GENERATED_BODY()

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Truong Sinh|Simulation")
    FGuid CommandId;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Truong Sinh|Simulation")
    FTruongSinhStableId BlueprintId;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Truong Sinh|Simulation")
    FTruongSinhStableId EffectId;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Truong Sinh|Simulation")
    int32 IntegrityBps = 0;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Truong Sinh|Simulation")
    int64 ExpiresAtMinute = 0;
};

/** One authored encounter may commit once, regardless of presentation or command retries. */
USTRUCT(BlueprintType)
struct TRUONGSINHSIMULATION_API FTruongSinhConflictRecord
{
    GENERATED_BODY()

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Truong Sinh|Simulation")
    FGuid CommandId;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Truong Sinh|Simulation")
    FTruongSinhStableId EncounterId;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Truong Sinh|Simulation")
    FTruongSinhStableId OpponentId;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Truong Sinh|Simulation")
    FTruongSinhStableId OutcomeId;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Truong Sinh|Simulation")
    int64 PermanentDamageDays = 0;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Truong Sinh|Simulation")
    bool bOpponentDefeated = false;
};

/** Minimal canonical state used to prove revision, time, RNG, idempotency and save round-trip. */
USTRUCT(BlueprintType)
struct TRUONGSINHSIMULATION_API FTruongSinhSimulationState
{
    GENERATED_BODY()

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Truong Sinh|Simulation")
    int32 SchemaVersion = 2;

    /** Absolute canonical time. One authored day is 1,440 minutes. */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Truong Sinh|Simulation")
    int64 ElapsedMinutes = 0;

    /** Real-time adapter remainder; game shutdown never adds to it. */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Truong Sinh|Simulation")
    int64 ExplorationRemainderMillis = 0;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Truong Sinh|Simulation")
    FTruongSinhStableId WorldLayerId;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Truong Sinh|Simulation")
    FTruongSinhSoulState Soul;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Truong Sinh|Simulation")
    FTruongSinhVesselState CurrentVessel;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Truong Sinh|Simulation")
    int64 WorldRevision = 0;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Truong Sinh|Simulation")
    FTruongSinhRngState Rng;

    /** Persisted so retrying a command after Continue cannot double-charge time. */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Truong Sinh|Simulation")
    TArray<FGuid> CommittedCommandIds;

    /** Canonical outputs from resolved activities; presentation never owns this inventory evidence. */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Truong Sinh|Simulation")
    TArray<FTruongSinhActivityOutputRecord> ActivityOutputRecords;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Truong Sinh|Simulation")
    TArray<FTruongSinhFormationState> Formations;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Truong Sinh|Simulation")
    TArray<FTruongSinhConflictRecord> ConflictRecords;
};

/** Pure deterministic transition engine. It never reads world, frame timing or presentation state. */
class TRUONGSINHSIMULATION_API FTruongSinhGameSimulation
{
public:
    static const TCHAR* AdvanceTimeActionId;
    static const TCHAR* CommitCultivationActionId;
    static const TCHAR* CommitResolvedActivityActionId;

    static FTruongSinhSimulationState CreateNewGame(int64 MasterSeed);
    static FTruongSinhActionResult Execute(
        FTruongSinhSimulationState& InOutState,
        const FTruongSinhActionCommand& Command);
    static FString ComputeStateHash(const FTruongSinhSimulationState& State);

private:
    static FTruongSinhActionResult Reject(
        const FTruongSinhSimulationState& State,
        const FTruongSinhActionCommand& Command,
        const TCHAR* ReasonId);
};

/** The sole runtime write gateway. World actors and UI submit commands here. */
UCLASS()
class TRUONGSINHSIMULATION_API UTruongSinhGameSimulationFacade : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;

    UFUNCTION(BlueprintCallable, Category = "Truong Sinh|Simulation")
    void StartNewGame(int64 MasterSeed);

    UFUNCTION(BlueprintCallable, Category = "Truong Sinh|Simulation")
    FTruongSinhActionResult Execute(const FTruongSinhActionCommand& Command);

    UFUNCTION(BlueprintPure, Category = "Truong Sinh|Simulation")
    FTruongSinhSimulationState GetState() const;

    /** Save-only boundary; not exposed to Blueprint gameplay. */
    void RestoreFromSave(const FTruongSinhSimulationState& LoadedState);

private:
    UPROPERTY()
    FTruongSinhSimulationState State;
};
