#pragma once

#include "CoreMinimal.h"
#include "BDFRPhotoModeTypes.generated.h"

UENUM(BlueprintType)
enum class EBDFRPhotoCameraMode : uint8
{
    Free UMETA(DisplayName="Free Camera"),
    Orbit UMETA(DisplayName="Orbit (Reserved)"),
    Selfie UMETA(DisplayName="Selfie")
};
