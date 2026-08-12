#pragma once

#include "CoreMinimal.h"
#include "Core/TruongSinhTypes.h"

/**
 * Versioned SplitMix64 stream used by canonical gameplay simulation.
 *
 * The algorithm and stream derivation are part of the save/replay contract.
 * Do not replace either without a save migration and updated golden vectors.
 */
class TRUONGSINHCORE_API FTruongSinhDeterministicRng
{
public:
    static constexpr int32 AlgorithmVersion = 1;

    static FTruongSinhRngStreamState CreateStream(int64 MasterSeed, const FString& StreamId);
    static uint64 NextUInt64(FTruongSinhRngStreamState& Stream);
    static int32 NextIntInclusive(FTruongSinhRngStreamState& Stream, int32 Minimum, int32 Maximum);

private:
    static uint64 HashStreamId(const FString& StreamId);
};
