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
};

/** Pure deterministic transition engine. It never reads world, frame timing or presentation state. */
class TRUONGSINHSIMULATION_API FTruongSinhGameSimulation
{
public:
    static const TCHAR* AdvanceTimeActionId;

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
