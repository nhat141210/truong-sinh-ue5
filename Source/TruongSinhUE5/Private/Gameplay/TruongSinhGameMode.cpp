#include "Gameplay/TruongSinhGameMode.h"

#include "Gameplay/TruongSinhCharacter.h"

ATruongSinhGameMode::ATruongSinhGameMode()
{
    DefaultPawnClass = ATruongSinhCharacter::StaticClass();
}
