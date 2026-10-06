#pragma once

#include "CoreMinimal.h"
#include "BDFRPhotoModeTypes.h"
#include "BDFRPhotoModePawn.h"
#include "Subsystems/LocalPlayerSubsystem.h"
#include "BDFRPhotoModeSubsystem.generated.h"

class AActor;
class AHUD;
class APawn;
class APlayerController;
class UBDFRPhotoPoseComponent;
class UBDFRPhotoPosePreset;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FBDFRPhotoModeEvent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FBDFRPhotoCameraModeChanged, EBDFRPhotoCameraMode, CameraMode);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FBDFRPhotoPoseCommandEvent, FName, PoseId);

UCLASS()
class BDFR_PHOTOMODE_API UBDFRPhotoModeSubsystem : public ULocalPlayerSubsystem
{
    GENERATED_BODY()

public:
    virtual void Deinitialize() override;

    UFUNCTION(BlueprintCallable, Category="BDFR|Photo Mode")
    bool EnterPhotoMode();

    UFUNCTION(BlueprintCallable, Category="BDFR|Photo Mode")
    void ExitPhotoMode();

    UFUNCTION(BlueprintCallable, Category="BDFR|Photo Mode")
    bool TogglePhotoMode();

    UFUNCTION(BlueprintPure, Category="BDFR|Photo Mode")
    bool IsPhotoModeActive() const { return bPhotoModeActive; }

    UFUNCTION(BlueprintPure, Category="BDFR|Photo Mode")
    ABDFRPhotoModePawn* GetPhotoPawn() const { return PhotoPawn.Get(); }

    UFUNCTION(BlueprintCallable, Category="BDFR|Photo Mode|Subject")
    bool SetPhotoSubject(AActor* NewSubject);

    UFUNCTION(BlueprintPure, Category="BDFR|Photo Mode|Subject")
    AActor* GetPhotoSubject() const { return PhotoSubject.Get(); }

    UFUNCTION(BlueprintCallable, Category="BDFR|Photo Mode|Camera")
    void SetFOV(float NewFOV);

    UFUNCTION(BlueprintCallable, Category="BDFR|Photo Mode|Camera")
    void SetCameraRoll(float RollDegrees)
    {
        if (ABDFRPhotoModePawn* Pawn = PhotoPawn.Get())
        {
            Pawn->SetCameraRoll(RollDegrees);
        }
    }

    UFUNCTION(BlueprintCallable, Category="BDFR|Photo Mode|Cinematic")
    void SetCinematicLens(float FocalLengthMm, float Aperture, float FocusDistanceCm)
    {
        if (ABDFRPhotoModePawn* Pawn = PhotoPawn.Get())
        {
            Pawn->SetCinematicLens(FocalLengthMm, Aperture, FocusDistanceCm);
        }
    }

    UFUNCTION(BlueprintCallable, Category="BDFR|Photo Mode|Camera")
    bool SetCameraMode(EBDFRPhotoCameraMode NewMode);

    UFUNCTION(BlueprintPure, Category="BDFR|Photo Mode|Camera")
    EBDFRPhotoCameraMode GetCameraMode() const;

    UFUNCTION(BlueprintCallable, Category="BDFR|Photo Mode|Selfie")
    bool EnterSelfieMode();

    UFUNCTION(BlueprintCallable, Category="BDFR|Photo Mode|Selfie")
    bool ExitSelfieMode();

    UFUNCTION(BlueprintCallable, Category="BDFR|Photo Mode|Selfie")
    void SetSelfieDistance(float NewDistance);

    UFUNCTION(BlueprintCallable, Category="BDFR|Photo Mode|Selfie")
    void SetSelfieOrbit(float YawOffset, float PitchOffset);

    UFUNCTION(BlueprintCallable, Category="BDFR|Photo Mode|Pose")
    bool ApplyPosePreset(UBDFRPhotoPosePreset* Preset);

    UFUNCTION(BlueprintCallable, Category="BDFR|Photo Mode|Pose")
    bool ApplyPoseById(FName PoseId);

    UFUNCTION(BlueprintCallable, Category="BDFR|Photo Mode|Pose")
    void ClearPose();

    UFUNCTION(BlueprintPure, Category="BDFR|Photo Mode|Pose")
    TArray<FName> GetAvailablePoseIds() const;

    UFUNCTION(BlueprintCallable, Category="BDFR|Photo Mode|Subject")
    bool LookSubjectAtCamera(bool bYawOnly = true);

    UFUNCTION(BlueprintCallable, Category="BDFR|Photo Mode")
    bool CaptureScreenshot(const FString& OptionalFilename = TEXT(""), bool bShowUI = false);

    UPROPERTY(BlueprintAssignable, Category="BDFR|Photo Mode")
    FBDFRPhotoModeEvent OnPhotoModeEntered;

    UPROPERTY(BlueprintAssignable, Category="BDFR|Photo Mode")
    FBDFRPhotoModeEvent OnPhotoModeExited;

    UPROPERTY(BlueprintAssignable, Category="BDFR|Photo Mode|Camera")
    FBDFRPhotoCameraModeChanged OnCameraModeChanged;

    UPROPERTY(BlueprintAssignable, Category="BDFR|Photo Mode|Pose")
    FBDFRPhotoPoseCommandEvent OnPoseApplied;

private:
    APlayerController* ResolvePlayerController() const;
    UBDFRPhotoPoseComponent* ResolvePoseComponent(bool bCreateIfMissing);
    UBDFRPhotoPosePreset* FindPosePreset(FName PoseId) const;
    void ReleasePoseComponent();
    void ConfigureSelfiePawn();

    UPROPERTY(Transient)
    TWeakObjectPtr<ABDFRPhotoModePawn> PhotoPawn;

    UPROPERTY(Transient)
    TWeakObjectPtr<APawn> PreviousPawn;

    UPROPERTY(Transient)
    TWeakObjectPtr<AHUD> PreviousHUD;

    UPROPERTY(Transient)
    TWeakObjectPtr<AActor> PhotoSubject;

    UPROPERTY(Transient)
    TObjectPtr<UBDFRPhotoPoseComponent> PoseComponent = nullptr;

    bool bOwnsPoseComponent = false;
    bool bPhotoModeActive = false;
    bool bWasPaused = false;
    bool bPreviousShowMouseCursor = false;
    bool bPreviousHUDVisible = true;
    bool bHasSubjectRotationSnapshot = false;
    FRotator SubjectRotationSnapshot = FRotator::ZeroRotator;
};
