#include "Gameplay/TruongSinhGameMode.h"

#include "Gameplay/TruongSinhCharacter.h"
#include "Gameplay/TruongSinhPlayerController.h"

ATruongSinhGameMode::ATruongSinhGameMode()
{
    DefaultPawnClass = ATruongSinhCharacter::StaticClass();
    PlayerControllerClass = ATruongSinhPlayerController::StaticClass();
}
