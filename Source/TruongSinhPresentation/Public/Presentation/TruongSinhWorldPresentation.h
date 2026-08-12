#pragma once

#include "CoreMinimal.h"
#include "Core/TruongSinhTypes.h"
#include "Combat/TruongSinhCombatSimulation.h"
#include "UObject/Interface.h"
#include "TruongSinhWorldPresentation.generated.h"

/** Resolved world state needed by zones, actors and UI to render a revision. */
USTRUCT(BlueprintType)
struct TRUONGSINHPRESENTATION_API FTruongSinhWorldPresentationSnapshot
{
    GENERATED_BODY()

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Truong Sinh|Presentation")
    int64 WorldRevision = 0;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Truong Sinh|Presentation")
    FTruongSinhStableId ZoneId;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Truong Sinh|Presentation")
    TArray<FTruongSinhStableId> ActiveWorldPatchIds;
};

/** Presentation receives resolved state and must not mutate the deterministic simulation. */
UINTERFACE(MinimalAPI, Blueprintable)
class UTruongSinhWorldPresentation : public UInterface
{
    GENERATED_BODY()
};

class TRUONGSINHPRESENTATION_API ITruongSinhWorldPresentation
{
    GENERATED_BODY()

public:
    virtual ~ITruongSinhWorldPresentation() = default;

    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Truong Sinh|Presentation")
    void ApplyWorldSnapshot(const FTruongSinhWorldPresentationSnapshot& Snapshot);

    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Truong Sinh|Presentation")
    void PresentActionResult(const FTruongSinhActionResult& Result);

    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Truong Sinh|Presentation")
    void PresentCombatReplay(const FTruongSinhCombatResult& Result);
};
