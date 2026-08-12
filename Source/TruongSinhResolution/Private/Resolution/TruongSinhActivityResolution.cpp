#include "Resolution/TruongSinhActivityResolution.h"

#include "Core/TruongSinhDeterministicRng.h"

namespace TruongSinhResolutionIds
{
constexpr TCHAR InvalidPlan[] = TEXT("activity.reject.invalid_plan");
constexpr TCHAR Eligible[] = TEXT("activity.preview.eligible");
constexpr TCHAR RiskLow[] = TEXT("activity.risk.low");
constexpr TCHAR RiskMedium[] = TEXT("activity.risk.medium");
constexpr TCHAR RiskHigh[] = TEXT("activity.risk.high");
constexpr TCHAR FactorBase[] = TEXT("activity.factor.base");
constexpr TCHAR FactorTechnique[] = TEXT("activity.factor.technique");
constexpr TCHAR FactorPreparation[] = TEXT("activity.factor.preparation");
constexpr TCHAR FactorEnvironment[] = TEXT("activity.factor.environment");
constexpr TCHAR FactorVariation[] = TEXT("activity.factor.seeded_variation");
constexpr TCHAR BeatApproach[] = TEXT("activity.beat.approach");
constexpr TCHAR BeatResolve[] = TEXT("activity.beat.resolve");
constexpr TCHAR BeatResult[] = TEXT("activity.beat.result");
constexpr TCHAR CueApproach[] = TEXT("cue.activity.approach.default");
constexpr TCHAR CueResolve[] = TEXT("cue.activity.resolve.default");
constexpr TCHAR CueResult[] = TEXT("cue.activity.result.default");

const TCHAR* ActivityPrefix(const ETruongSinhActivityType Type)
{
    switch (Type)
    {
    case ETruongSinhActivityType::Breakthrough:
        return TEXT("breakthrough");
    case ETruongSinhActivityType::Alchemy:
        return TEXT("alchemy");
    case ETruongSinhActivityType::Formation:
        return TEXT("formation");
    case ETruongSinhActivityType::Conflict:
        return TEXT("conflict");
    default:
        return TEXT("cultivation");
    }
}
}

namespace
{
FTruongSinhStableId MakeId(const TCHAR* Value)
{
    FTruongSinhStableId Id;
    Id.Value = Value;
    return Id;
}

bool IsPlanValid(const FTruongSinhActivityPlan& Plan)
{
    if (!Plan.Action.CommandId.IsValid() || !Plan.Action.ActionId.IsValid() ||
        !Plan.ActivityId.IsValid() || !Plan.MethodId.IsValid() ||
        !Plan.LocationId.IsValid() || !Plan.Action.InstigatorId.IsValid() ||
        Plan.DurationMinutes <= 0)
    {
        return false;
    }
    for (const FTruongSinhActivityResource& Resource : Plan.Resources)
    {
        if (!Resource.ResourceId.IsValid() || Resource.Units <= 0)
        {
            return false;
        }
    }
    return true;
}

int32 StrategyModifierUnits(const ETruongSinhActivityStrategy Strategy)
{
    switch (Strategy)
    {
    case ETruongSinhActivityStrategy::Safe:
    case ETruongSinhActivityStrategy::Cautious:
        return 250;
    case ETruongSinhActivityStrategy::Forceful:
    case ETruongSinhActivityStrategy::Overwhelm:
        return 150;
    case ETruongSinhActivityStrategy::Endure:
        return 100;
    case ETruongSinhActivityStrategy::Retreat:
        return -300;
    default:
        return 0;
    }
}

void AddFactor(TArray<FTruongSinhResolutionFactor>& Factors, const TCHAR* Id, const int64 Units)
{
    FTruongSinhResolutionFactor Factor;
    Factor.FactorId = MakeId(Id);
    Factor.Units = Units;
    Factors.Add(MoveTemp(Factor));
}

void AddBeat(
    TArray<FTruongSinhPresentationBeat>& Beats,
    const int32 Sequence,
    const TCHAR* BeatId,
    const TCHAR* CueId,
    const int64 Magnitude)
{
    FTruongSinhPresentationBeat Beat;
    Beat.Sequence = Sequence;
    Beat.BeatId = MakeId(BeatId);
    Beat.PresentationCueId = MakeId(CueId);
    Beat.MagnitudeUnits = Magnitude;
    Beats.Add(MoveTemp(Beat));
}
}

FTruongSinhActivityPreview FTruongSinhAutoResolver::Preview(
    const FTruongSinhActivitySnapshot& Snapshot,
    const FTruongSinhActivityPlan& Plan)
{
    FTruongSinhActivityPreview Preview;
    Preview.DurationMinutes = Plan.DurationMinutes;
    Preview.bEligible = IsPlanValid(Plan) && Snapshot.PerformerPower >= 0 &&
        Snapshot.DifficultyOrTargetPower >= 0;
    Preview.ReasonId = MakeId(Preview.bEligible
        ? TruongSinhResolutionIds::Eligible
        : TruongSinhResolutionIds::InvalidPlan);

    const int64 Gap = Snapshot.PerformerPower - Snapshot.DifficultyOrTargetPower;
    Preview.RiskBandId = MakeId(Gap >= 2500
        ? TruongSinhResolutionIds::RiskLow
        : Gap >= -1000
            ? TruongSinhResolutionIds::RiskMedium
            : TruongSinhResolutionIds::RiskHigh);
    return Preview;
}

FTruongSinhAutoResolutionResult FTruongSinhAutoResolver::Resolve(
    const FTruongSinhActivitySnapshot& Snapshot,
    const FTruongSinhActivityPlan& Plan)
{
    FTruongSinhAutoResolutionResult Result;
    const FTruongSinhActivityPreview Preview = FTruongSinhAutoResolver::Preview(Snapshot, Plan);
    if (!Preview.bEligible)
    {
        Result.ReasonId = MakeId(TruongSinhResolutionIds::InvalidPlan);
        return Result;
    }

    const int64 Strategy = StrategyModifierUnits(Plan.Strategy);
    const FString CommandToken = Plan.Action.CommandId.ToString(EGuidFormats::Digits).ToLower();
    const FString StreamId = FString::Printf(TEXT("resolution.%s"), *CommandToken);
    FTruongSinhRngStreamState Stream =
        FTruongSinhDeterministicRng::CreateStream(Snapshot.MasterSeed, StreamId);
    const int64 Variation = FTruongSinhDeterministicRng::NextIntInclusive(Stream, -300, 300);

    Result.TargetScore = Snapshot.DifficultyOrTargetPower;
    Result.FinalScore = Snapshot.PerformerPower + Snapshot.TechniqueModifierUnits +
        Snapshot.PreparationModifierUnits + Snapshot.EnvironmentModifierUnits + Strategy + Variation;
    Result.TimeAdvancedMinutes = Plan.DurationMinutes;

    AddFactor(Result.Factors, TruongSinhResolutionIds::FactorBase, Snapshot.PerformerPower);
    AddFactor(Result.Factors, TruongSinhResolutionIds::FactorTechnique, Snapshot.TechniqueModifierUnits);
    AddFactor(Result.Factors, TruongSinhResolutionIds::FactorPreparation, Snapshot.PreparationModifierUnits + Strategy);
    AddFactor(Result.Factors, TruongSinhResolutionIds::FactorEnvironment, Snapshot.EnvironmentModifierUnits);
    AddFactor(Result.Factors, TruongSinhResolutionIds::FactorVariation, Variation);

    const int64 Gap = Result.FinalScore - Result.TargetScore;
    if (Gap >= 2500)
    {
        Result.Outcome = ETruongSinhResolutionOutcome::GreatSuccess;
    }
    else if (Gap >= 0)
    {
        Result.Outcome = ETruongSinhResolutionOutcome::Success;
    }
    else if (Gap >= -1000)
    {
        Result.Outcome = ETruongSinhResolutionOutcome::PartialSuccess;
    }
    else
    {
        Result.Outcome = ETruongSinhResolutionOutcome::Failure;
    }

    switch (Plan.Type)
    {
    case ETruongSinhActivityType::Breakthrough:
        switch (Result.Outcome)
        {
        case ETruongSinhResolutionOutcome::GreatSuccess:
            Result.RealmLifespanBonusDays = 20 * 365;
            Result.NewRealmId.Value = TEXT("realm.foundation");
            break;
        case ETruongSinhResolutionOutcome::Success:
            Result.RealmLifespanBonusDays = 10 * 365;
            Result.NewRealmId.Value = TEXT("realm.foundation");
            break;
        case ETruongSinhResolutionOutcome::PartialSuccess:
            Result.CultivationProgressUnits = 250;
            break;
        default:
            break;
        }
        break;
    default:
        switch (Result.Outcome)
        {
        case ETruongSinhResolutionOutcome::GreatSuccess:
            Result.CultivationProgressUnits = 1200;
            break;
        case ETruongSinhResolutionOutcome::Success:
            Result.CultivationProgressUnits = 800;
            break;
        case ETruongSinhResolutionOutcome::PartialSuccess:
            Result.CultivationProgressUnits = 350;
            break;
        default:
            break;
        }
        break;
    }
    const TCHAR* Prefix = TruongSinhResolutionIds::ActivityPrefix(Plan.Type);
    const TCHAR* Outcome = Result.Outcome == ETruongSinhResolutionOutcome::GreatSuccess ? TEXT("great_success") :
        Result.Outcome == ETruongSinhResolutionOutcome::Success ? TEXT("success") :
        Result.Outcome == ETruongSinhResolutionOutcome::PartialSuccess ? TEXT("partial_success") : TEXT("failure");
    Result.OutcomeId.Value = FString::Printf(TEXT("%s.outcome.%s"), Prefix, Outcome);
    Result.ReplayId.Value = FString::Printf(TEXT("replay.%s.%s"), Prefix,
        *Plan.Action.CommandId.ToString(EGuidFormats::Digits).ToLower());

    AddBeat(Result.Beats, 0, TruongSinhResolutionIds::BeatApproach,
        TruongSinhResolutionIds::CueApproach, Snapshot.PerformerPower);
    AddBeat(Result.Beats, 1, TruongSinhResolutionIds::BeatResolve,
        TruongSinhResolutionIds::CueResolve, Gap);
    AddBeat(Result.Beats, 2, TruongSinhResolutionIds::BeatResult,
        TruongSinhResolutionIds::CueResult, static_cast<int64>(Result.Outcome));
    return Result;
}

FTruongSinhActivityPreview UTruongSinhAutoResolutionSubsystem::PreviewActivity(
    const FTruongSinhActivitySnapshot& Snapshot,
    const FTruongSinhActivityPlan& Plan) const
{
    return FTruongSinhAutoResolver::Preview(Snapshot, Plan);
}
