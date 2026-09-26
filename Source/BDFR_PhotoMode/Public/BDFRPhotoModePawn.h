#pragma once

#include "CoreMinimal.h"
#include "BDFRPhotoModeTypes.h"
#include "GameFramework/Pawn.h"
#include "BDFRPhotoModePawn.generated.h"

class AActor;
class UCameraComponent;
class USceneComponent;

UCLASS(BlueprintType)
class BDFR_PHOTOMODE_API ABDFRPhotoModePawn : public APawn
{
    GENERATED_BODY()

public:
    ABDFRPhotoModePawn();

    virtual void Tick(float DeltaSeconds) override;

    UFUNCTION(BlueprintCallable, Category="BDFR|Photo Mode|Camera")
    void SetPhotoFOV(float NewFOV);

    UFUNCTION(BlueprintPure, Category="BDFR|Photo Mode|Camera")
    float GetPhotoFOV() const;

    UFUNCTION(BlueprintCallable, Category="BDFR|Photo Mode|Camera")
    void SetMoveSpeed(float NewSpeed);

    UFUNCTION(BlueprintCallable, Category="BDFR|Photo Mode|Camera")
    void SetCameraMode(EBDFRPhotoCameraMode NewMode);

    UFUNCTION(BlueprintPure, Category="BDFR|Photo Mode|Camera")
    EBDFRPhotoCameraMode GetCameraMode() const { return CameraMode; }

    UFUNCTION(BlueprintCallable, Category="BDFR|Photo Mode|Selfie")
    void ConfigureSelfie(AActor* InSubject, float InDistance, float InHorizontalOffset, float InVerticalOffset, FName InAnchorSocket, float InSelfieFOV, bool bInSmooth, float InSmoothingSpeed);

    UFUNCTION(BlueprintCallable, Category="BDFR|Photo Mode|Selfie")
    void SetSelfieDistance(float NewDistance);

    UFUNCTION(BlueprintCallable, Category="BDFR|Photo Mode|Selfie")
    void SetSelfieOrbit(float YawOffset, float PitchOffset);

    UFUNCTION(BlueprintCallable, Category="BDFR|Photo Mode|Selfie")
    void SnapSelfieCamera();

    UFUNCTION(BlueprintPure, Category="BDFR|Photo Mode|Camera")
    UCameraComponent* GetPhotoCamera() const { return Camera; }

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="BDFR|Photo Mode")
    TObjectPtr<USceneComponent> SceneRoot;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="BDFR|Photo Mode")
    TObjectPtr<UCameraComponent> Camera;

private:
    void TickFreeCamera(class APlayerController* PC, float EffectiveDelta);
    void TickSelfieCamera(class APlayerController* PC, float EffectiveDelta);
    FVector ResolveSelfieAnchor() const;

    EBDFRPhotoCameraMode CameraMode = EBDFRPhotoCameraMode::Free;
    TWeakObjectPtr<AActor> SelfieSubject;

    float CurrentMoveSpeed = 900.0f;
    float LookSensitivity = 0.12f;
    float FastMoveMultiplier = 4.0f;
    float SpeedStep = 150.0f;
    float MinFOV = 15.0f;
    float MaxFOV = 120.0f;

    float SelfieDistance = 95.0f;
    float MinSelfieDistance = 40.0f;
    float MaxSelfieDistance = 220.0f;
    float SelfieHorizontalOffset = 18.0f;
    float SelfieVerticalOffset = 8.0f;
    float SelfieYawOffset = 0.0f;
    float SelfiePitchOffset = 0.0f;
    float SelfieFOV = 65.0f;
    FName SelfieAnchorSocket = TEXT("head");
    bool bSmoothSelfieCamera = true;
    float SelfieSmoothingSpeed = 12.0f;
    float PreSelfieFOV = 70.0f;
};
