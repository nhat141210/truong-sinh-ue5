#include "Simulation/TruongSinhLifeState.h"

#include "Core/TruongSinhDeterministicRng.h"

namespace TruongSinhLifeIds
{
constexpr TCHAR InvalidRequest[] = TEXT("soul.possession.reject.invalid_request");
constexpr TCHAR Success[] = TEXT("soul.possession.success");
constexpr TCHAR Failure[] = TEXT("soul.possession.failure");
constexpr TCHAR Emergency[] = TEXT("soul.possession.emergency_vessel");
constexpr TCHAR PlayerSoul[] = TEXT("soul.player");
constexpr TCHAR OriginVessel[] = TEXT("vessel.player_origin");
constexpr TCHAR OriginIdentity[] = TEXT("identity.player_origin");
constexpr TCHAR MortalRealm[] = TEXT("realm.mortal");
constexpr TCHAR MixedRoot[] = TEXT("root.mixed");
constexpr TCHAR EmergencyVessel[] = TEXT("vessel.wandering_soul_fallback");
constexpr TCHAR EmergencyIdentity[] = TEXT("identity.unknown_wanderer");
}

namespace
{
FTruongSinhStableId MakeId(const TCHAR* Value)
{
    FTruongSinhStableId Id;
    Id.Value = Value;
    return Id;
}

FTruongSinhVesselState MakeEmergencyVessel()
{
    FTruongSinhVesselState Vessel;
    Vessel.VesselId = MakeId(TruongSinhLifeIds::EmergencyVessel);
    Vessel.IdentityId = MakeId(TruongSinhLifeIds::EmergencyIdentity);
    Vessel.SpiritualRootId = MakeId(TruongSinhLifeIds::MixedRoot);
    Vessel.RealmId = MakeId(TruongSinhLifeIds::MortalRealm);
    Vessel.CultivationUnits = 0;
    Vessel.Lifespan.BiologicalAgeDays = 18 * 365;
    Vessel.Lifespan.BaseLifespanDays = 55 * 365;
    return Vessel;
}

int64 SaturatingAddNonNegative(const int64 Left, const int64 Right)
{
    const int64 SafeLeft = FMath::Max<int64>(0, Left);
    const int64 SafeRight = FMath::Max<int64>(0, Right);
    return SafeLeft > MAX_int64 - SafeRight ? MAX_int64 : SafeLeft + SafeRight;
}
}

int64 FTruongSinhLifespanState::EffectiveLifespanDays() const
{
    int64 Positive = SaturatingAddNonNegative(BaseLifespanDays, RealmBonusDays);
    Positive = SaturatingAddNonNegative(Positive, TechniqueBonusDays);
    Positive = SaturatingAddNonNegative(Positive, PillAndResourceBonusDays);
    return FMath::Max<int64>(0, Positive - FMath::Max<int64>(0, PermanentDamageDays));
}

int64 FTruongSinhLifespanState::RemainingLifespanDays() const
{
    return FMath::Max<int64>(0, EffectiveLifespanDays() - FMath::Max<int64>(0, BiologicalAgeDays));
}

bool FTruongSinhLifespanState::IsExpired() const
{
    return RemainingLifespanDays() == 0;
}

FTruongSinhSoulState FTruongSinhLifeRules::CreateInitialSoul()
{
    FTruongSinhSoulState Soul;
    Soul.SoulId = MakeId(TruongSinhLifeIds::PlayerSoul);
    Soul.IntegrityUnits = 10000;
    Soul.VesselHistoryIds.Add(MakeId(TruongSinhLifeIds::OriginVessel));
    return Soul;
}

FTruongSinhVesselState FTruongSinhLifeRules::CreateInitialVessel()
{
    FTruongSinhVesselState Vessel;
    Vessel.VesselId = MakeId(TruongSinhLifeIds::OriginVessel);
    Vessel.IdentityId = MakeId(TruongSinhLifeIds::OriginIdentity);
    Vessel.SpiritualRootId = MakeId(TruongSinhLifeIds::MixedRoot);
    Vessel.RealmId = MakeId(TruongSinhLifeIds::MortalRealm);
    Vessel.Lifespan.BiologicalAgeDays = 18 * 365;
    Vessel.Lifespan.BaseLifespanDays = 70 * 365;
    return Vessel;
}

FTruongSinhPossessionResult FTruongSinhLifeRules::ResolvePossession(
    const FTruongSinhSoulState& Soul,
    const FTruongSinhVesselState& CurrentVessel,
    const FTruongSinhVesselState& TargetVessel,
    const FTruongSinhPossessionRequest& Request)
{
    FTruongSinhPossessionResult Result;
    Result.Soul = Soul;
    Result.Vessel = CurrentVessel;

    if (!Request.CommandId.IsValid() || !Request.PossessionTechniqueId.IsValid() ||
        !Request.MediumId.IsValid() || !Soul.SoulId.IsValid() ||
        !TargetVessel.VesselId.IsValid() || Request.CompatibilityBps < 0 ||
        Request.CompatibilityBps > 10000 || Soul.IntegrityUnits <= 0)
    {
        Result.ReasonId = MakeId(TruongSinhLifeIds::InvalidRequest);
        return Result;
    }

    const FString StreamId = FString::Printf(
        TEXT("possession.%s"),
        *Request.CommandId.ToString(EGuidFormats::Digits).ToLower());
    FTruongSinhRngStreamState Stream =
        FTruongSinhDeterministicRng::CreateStream(Request.MasterSeed, StreamId);
    const int32 Roll = FTruongSinhDeterministicRng::NextIntInclusive(Stream, 0, 9999);
    const int32 Score = FMath::Clamp(
        (Request.CompatibilityBps * 7 + Soul.IntegrityUnits * 3) / 10,
        0,
        10000);

    if (Roll < Score)
    {
        Result.Outcome = ETruongSinhPossessionOutcome::Succeeded;
        Result.ReasonId = MakeId(TruongSinhLifeIds::Success);
        Result.Soul.IntegrityUnits = FMath::Max(0, Soul.IntegrityUnits - 500);
        if (!Result.Soul.VesselHistoryIds.Contains(TargetVessel.VesselId))
        {
            Result.Soul.VesselHistoryIds.Add(TargetVessel.VesselId);
        }
        Result.Vessel = TargetVessel;
        Result.Vessel.RealmId = MakeId(TruongSinhLifeIds::MortalRealm);
        Result.Vessel.CultivationUnits = 0;
        return Result;
    }

    Result.Soul.IntegrityUnits = FMath::Max(0, Soul.IntegrityUnits - 2000);
    if (Request.bCurrentBodyUnavailable)
    {
        Result.Outcome = ETruongSinhPossessionOutcome::EmergencyVessel;
        Result.ReasonId = MakeId(TruongSinhLifeIds::Emergency);
        Result.Vessel = MakeEmergencyVessel();
        if (!Result.Soul.VesselHistoryIds.Contains(Result.Vessel.VesselId))
        {
            Result.Soul.VesselHistoryIds.Add(Result.Vessel.VesselId);
        }
    }
    else
    {
        Result.Outcome = ETruongSinhPossessionOutcome::Failed;
        Result.ReasonId = MakeId(TruongSinhLifeIds::Failure);
    }
    return Result;
}
