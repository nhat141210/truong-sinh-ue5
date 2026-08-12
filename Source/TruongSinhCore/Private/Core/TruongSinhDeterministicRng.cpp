#include "Core/TruongSinhDeterministicRng.h"

#include "Containers/StringConv.h"

namespace TruongSinhRng
{
constexpr uint64 FnvOffsetBasis = 14695981039346656037ull;
constexpr uint64 FnvPrime = 1099511628211ull;
constexpr uint64 GoldenGamma = 0x9E3779B97F4A7C15ull;
}

FTruongSinhRngStreamState FTruongSinhDeterministicRng::CreateStream(
    const int64 MasterSeed,
    const FString& StreamId)
{
    FTruongSinhStableId CanonicalStreamId;
    CanonicalStreamId.Value = StreamId;
    checkf(CanonicalStreamId.IsValid(), TEXT("A deterministic RNG stream requires a canonical lowercase stream ID."));

    FTruongSinhRngStreamState Stream;
    Stream.StreamId = StreamId;
    Stream.State = static_cast<uint64>(MasterSeed) ^ HashStreamId(StreamId) ^ TruongSinhRng::GoldenGamma;
    Stream.DrawCount = 0;
    return Stream;
}

uint64 FTruongSinhDeterministicRng::NextUInt64(FTruongSinhRngStreamState& Stream)
{
    Stream.State += TruongSinhRng::GoldenGamma;
    uint64 Value = Stream.State;
    Value = (Value ^ (Value >> 30)) * 0xBF58476D1CE4E5B9ull;
    Value = (Value ^ (Value >> 27)) * 0x94D049BB133111EBull;
    Value ^= Value >> 31;
    ++Stream.DrawCount;
    return Value;
}

int32 FTruongSinhDeterministicRng::NextIntInclusive(
    FTruongSinhRngStreamState& Stream,
    const int32 Minimum,
    const int32 Maximum)
{
    checkf(Minimum <= Maximum, TEXT("Minimum must be less than or equal to Maximum."));

    const uint64 Range = static_cast<uint64>(static_cast<int64>(Maximum) - Minimum) + 1ull;
    const uint64 RejectionThreshold = (0ull - Range) % Range;

    uint64 Sample = 0;
    do
    {
        Sample = NextUInt64(Stream);
    }
    while (Sample < RejectionThreshold);

    return static_cast<int32>(static_cast<int64>(Minimum) + static_cast<int64>(Sample % Range));
}

uint64 FTruongSinhDeterministicRng::HashStreamId(const FString& StreamId)
{
    const FTCHARToUTF8 Utf8(*StreamId);

    uint64 Hash = TruongSinhRng::FnvOffsetBasis;
    for (int32 Index = 0; Index < Utf8.Length(); ++Index)
    {
        Hash ^= static_cast<uint8>(Utf8.Get()[Index]);
        Hash *= TruongSinhRng::FnvPrime;
    }
    return Hash;
}
