#include "TruongSinhInteractionProvider.h"

bool FTruongSinhInteractionSelection::SelectBestOffer(
    const TArray<FTruongSinhInteractionOffer>& Offers,
    const float DistanceCentimeters,
    FTruongSinhInteractionOffer& OutOffer)
{
    OutOffer = FTruongSinhInteractionOffer();
    if (!FMath::IsFinite(DistanceCentimeters) || DistanceCentimeters < 0.0f)
    {
        return false;
    }

    const FTruongSinhInteractionOffer* Best = nullptr;
    for (const FTruongSinhInteractionOffer& Candidate : Offers)
    {
        if (!Candidate.bEnabled || !Candidate.CandidateId.IsValid() ||
            !FMath::IsFinite(Candidate.MaximumRangeCentimeters) ||
            Candidate.MaximumRangeCentimeters < 0.0f ||
            DistanceCentimeters > Candidate.MaximumRangeCentimeters)
        {
            continue;
        }

        if (!Best || Candidate.Priority > Best->Priority ||
            (Candidate.Priority == Best->Priority && Candidate.CandidateId.Value < Best->CandidateId.Value))
        {
            Best = &Candidate;
        }
    }

    if (!Best)
    {
        return false;
    }

    OutOffer = *Best;
    return true;
}
