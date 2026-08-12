#include "Data/TruongSinhActivityRegistryDataAsset.h"

const FTruongSinhActivityDefinition* UTruongSinhActivityRegistryDataAsset::FindByFacility(
    const FTruongSinhStableId& FacilityId) const
{
    const FTruongSinhActivityDefinition* Found = nullptr;
    for (const FTruongSinhActivityDefinition& Definition : Definitions)
    {
        if (!(Definition.FacilityId == FacilityId))
        {
            continue;
        }
        if (!Found || Definition.ActivityId.LexicalLess(Found->ActivityId))
        {
            Found = &Definition;
        }
    }
    return Found;
}

bool UTruongSinhActivityRegistryDataAsset::ValidateRegistry(FString& OutError) const
{
    OutError.Reset();
    TSet<FString> FacilityIds;
    TSet<FString> ActivityIds;
    for (const FTruongSinhActivityDefinition& Definition : Definitions)
    {
        if (!Definition.ActivityId.IsValid() || !Definition.FacilityId.IsValid() ||
            !Definition.MethodId.IsValid() || !Definition.LocationId.IsValid() ||
            Definition.ResolverId.IsNone() || Definition.DurationMinutes <= 0 ||
            Definition.MinimumCultivationUnits < 0 || Definition.DifficultyOrTargetPower < 0)
        {
            OutError = TEXT("Activity registry contains an invalid definition");
            return false;
        }
        if ((Definition.ResolverId == TEXT("alchemy")) != Definition.OutputId.IsValid() ||
            (Definition.OutputId.IsValid() && Definition.MaximumOutputUnits <= 0) ||
            (!Definition.OutputId.IsValid() && Definition.MaximumOutputUnits != 0))
        {
            OutError = TEXT("Alchemy output definition is incomplete or unexpected");
            return false;
        }
        if ((Definition.ResolverId == TEXT("formation")) != Definition.FormationEffectId.IsValid() ||
            (Definition.FormationEffectId.IsValid() && Definition.FormationDurationMinutes <= 0) ||
            (!Definition.FormationEffectId.IsValid() && Definition.FormationDurationMinutes != 0))
        {
            OutError = TEXT("Formation effect definition is incomplete or unexpected");
            return false;
        }
        if ((Definition.ResolverId == TEXT("conflict")) != Definition.ConflictOpponentId.IsValid())
        {
            OutError = TEXT("Conflict opponent definition is incomplete or unexpected");
            return false;
        }
        if (FacilityIds.Contains(Definition.FacilityId.Value) || ActivityIds.Contains(Definition.ActivityId.Value))
        {
            OutError = TEXT("Activity registry contains a duplicate facility or activity ID");
            return false;
        }
        FacilityIds.Add(Definition.FacilityId.Value);
        ActivityIds.Add(Definition.ActivityId.Value);
    }
    return true;
}
