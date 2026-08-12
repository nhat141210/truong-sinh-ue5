#include "Core/TruongSinhGameInstance.h"
#include "TruongSinhUE5.h"

void UTruongSinhGameInstance::Init()
{
    Super::Init();
    UE_LOG(LogTruongSinh, Log, TEXT("Truong Sinh bootstrap initialized."));
}

void UTruongSinhGameInstance::Shutdown()
{
    UE_LOG(LogTruongSinh, Log, TEXT("Truong Sinh bootstrap shutting down."));
    Super::Shutdown();
}
