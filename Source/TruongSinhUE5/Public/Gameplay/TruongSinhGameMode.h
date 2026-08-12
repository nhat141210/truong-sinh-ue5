#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "TruongSinhGameMode.generated.h"

/**
 * M1 bootstrap GameMode. It owns only the third-person shell; progression and
 * simulation state remain in game-instance subsystems and canonical data.
 */
UCLASS()
class TRUONGSINHUE5_API ATruongSinhGameMode : public AGameModeBase
{
    GENERATED_BODY()

public:
    ATruongSinhGameMode();
};
