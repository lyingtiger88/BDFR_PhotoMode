#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "BDFRPhotoModeSettings.generated.h"

class UBDFRPhotoPosePreset;

UCLASS(Config=Game, DefaultConfig, meta=(DisplayName="BDFR Photo Mode"))
class BDFR_PHOTOMODE_API UBDFRPhotoModeSettings : public UDeveloperSettings
{
    GENERATED_BODY()

public:
    UBDFRPhotoModeSettings();

    UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category="General")
    bool bFreezeWorldOnEnter = true;

    UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category="General")
    bool bHideHUDOnEnter = true;

    UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category="General")
    bool bRestoreSubjectRotationOnExit = true;

    UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category="Camera", meta=(ClampMin="1.0", UIMin="1.0"))
    float MoveSpeed = 900.0f;

    UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category="Camera", meta=(ClampMin="1.0", UIMin="1.0"))
    float FastMoveMultiplier = 4.0f;

    UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category="Camera", meta=(ClampMin="0.01", UIMin="0.01"))
    float LookSensitivity = 0.12f;

    UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category="Camera", meta=(ClampMin="5.0", ClampMax="170.0"))
    float DefaultFOV = 70.0f;

    UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category="Camera", meta=(ClampMin="1.0", ClampMax="170.0"))
    float MinFOV = 15.0f;

    UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category="Camera", meta=(ClampMin="1.0", ClampMax="170.0"))
    float MaxFOV = 120.0f;

    UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category="Camera", meta=(ClampMin="0.01", UIMin="0.01"))
    float SpeedStep = 150.0f;

    UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category="Pose")
    TArray<TSoftObjectPtr<UBDFRPhotoPosePreset>> PosePresets;

    UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category="Selfie", meta=(ClampMin="20.0", UIMin="20.0"))
    float SelfieDistance = 95.0f;

    UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category="Selfie", meta=(ClampMin="20.0"))
    float MinSelfieDistance = 40.0f;

    UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category="Selfie", meta=(ClampMin="20.0"))
    float MaxSelfieDistance = 220.0f;

    UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category="Selfie")
    float SelfieHorizontalOffset = 18.0f;

    UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category="Selfie")
    float SelfieVerticalOffset = 8.0f;

    UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category="Selfie", meta=(ClampMin="5.0", ClampMax="170.0"))
    float SelfieFOV = 65.0f;

    UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category="Selfie")
    FName SelfieAnchorSocket = TEXT("head");

    UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category="Selfie")
    bool bSmoothSelfieCamera = true;

    UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category="Selfie", meta=(ClampMin="0.1"))
    float SelfieSmoothingSpeed = 12.0f;

    UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category="Selfie")
    bool bAutoLookAtCameraInSelfie = true;
};
