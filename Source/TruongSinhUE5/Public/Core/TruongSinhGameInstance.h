#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "TruongSinhGameInstance.generated.h"

/** Entry point for subsystems. Gameplay state must not live in maps, widgets or animation blueprints. */
UCLASS()
class TRUONGSINHUE5_API UTruongSinhGameInstance : public UGameInstance
{
    GENERATED_BODY()

public:
    virtual void Init() override;
    virtual void Shutdown() override;
};
