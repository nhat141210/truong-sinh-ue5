#pragma once

#include "CoreMinimal.h"
#include "StructUtils/InstancedStruct.h"
#include "TruongSinhTypes.generated.h"

/** Stable authored identifiers are used by data, tests and saves. Never derive them from display text. */
USTRUCT(BlueprintType)
struct TRUONGSINHCORE_API FTruongSinhStableId
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Truong Sinh")
    FString Value;

    bool IsValid() const
    {
        if (Value.IsEmpty())
        {
            return false;
        }

        for (const TCHAR Character : Value)
        {
            const bool bAllowed =
                (Character >= TEXT('a') && Character <= TEXT('z')) ||
                (Character >= TEXT('0') && Character <= TEXT('9')) ||
                Character == TEXT('.') || Character == TEXT('_') || Character == TEXT('-');
            if (!bAllowed)
            {
                return false;
            }
        }
        return true;
    }

    bool LexicalLess(const FTruongSinhStableId& Other) const
    {
        return Value.Compare(Other.Value, ESearchCase::CaseSensitive) < 0;
    }

    friend bool operator==(const FTruongSinhStableId& Left, const FTruongSinhStableId& Right)
    {
        return Left.Value == Right.Value;
    }

    friend uint32 GetTypeHash(const FTruongSinhStableId& Id)
    {
        return GetTypeHash(Id.Value);
    }
};

/** Empty reflected base for typed action payloads declared by domain modules. */
USTRUCT(BlueprintType)
struct TRUONGSINHCORE_API FTruongSinhActionPayload
{
    GENERATED_BODY()
};

/**
 * A canonical gameplay command. Presentation layers may request it, but only
 * a simulation subsystem may accept it and advance the world revision.
 */
USTRUCT(BlueprintType)
struct TRUONGSINHCORE_API FTruongSinhActionCommand
{
    GENERATED_BODY()

    /** Correlation/idempotency ID. It is journaled but never used as gameplay entropy. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Truong Sinh")
    FGuid CommandId;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Truong Sinh")
    FTruongSinhStableId ActionId;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Truong Sinh")
    FTruongSinhStableId InstigatorId;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Truong Sinh")
    TArray<FTruongSinhStableId> TargetIds;

    /** A reflected, typed domain payload. JSON blobs and untyped key/value bags are forbidden. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Truong Sinh")
    FInstancedStruct Payload;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Truong Sinh")
    int64 ExpectedWorldRevision = 0;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Truong Sinh")
    int64 Sequence = 0;
};

UENUM(BlueprintType)
enum class ETruongSinhActionStatus : uint8
{
    Rejected,
    Committed
};

/** Ordered, immutable event emitted by a committed canonical action. */
USTRUCT(BlueprintType)
struct TRUONGSINHCORE_API FTruongSinhDomainEvent
{
    GENERATED_BODY()

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Truong Sinh")
    FTruongSinhStableId EventTypeId;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Truong Sinh")
    int64 Sequence = 0;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Truong Sinh")
    FInstancedStruct Payload;
};

/** Immutable outcome returned by the simulation and consumed by presentation. */
USTRUCT(BlueprintType)
struct TRUONGSINHCORE_API FTruongSinhActionResult
{
    GENERATED_BODY()

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Truong Sinh")
    ETruongSinhActionStatus Status = ETruongSinhActionStatus::Rejected;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Truong Sinh")
    FTruongSinhStableId ActionId;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Truong Sinh")
    FTruongSinhStableId ReasonId;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Truong Sinh")
    int64 PreviousWorldRevision = 0;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Truong Sinh")
    int64 NewWorldRevision = 0;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Truong Sinh")
    TArray<FTruongSinhDomainEvent> Events;

    /** Canonical simulation hash after commit; empty for rejected commands. */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Truong Sinh")
    FString StateHash;
};

UENUM(BlueprintType)
enum class ETruongSinhElement : uint8
{
    Metal,
    Wood,
    Water,
    Fire,
    Earth
};

/** Calendar normalization rules are authored/audited data, not level time. */
USTRUCT(BlueprintType)
struct TRUONGSINHCORE_API FTruongSinhGameDate
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Truong Sinh")
    int32 Year = 1;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Truong Sinh")
    int32 Month = 1;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Truong Sinh")
    int32 Day = 1;

    FString ToDebugString() const
    {
        return FString::Printf(TEXT("Y%d-M%d-D%d"), Year, Month, Day);
    }
};

USTRUCT(BlueprintType)
struct TRUONGSINHCORE_API FTruongSinhRngStreamState
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Truong Sinh")
    FString StreamId;

    /** Canonical SplitMix64 bits. Intentionally hidden from Blueprint pins. */
    UPROPERTY()
    uint64 State = 0;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Truong Sinh")
    int64 DrawCount = 0;
};

/**
 * Saveable random state. New streams must be derived from MasterSeed and a
 * stable StreamId; never from frame time, map state or a display string.
 */
USTRUCT(BlueprintType)
struct TRUONGSINHCORE_API FTruongSinhRngState
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Truong Sinh")
    int32 AlgorithmVersion = 1;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Truong Sinh")
    int64 MasterSeed = 0;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Truong Sinh")
    TArray<FTruongSinhRngStreamState> Streams;
};
