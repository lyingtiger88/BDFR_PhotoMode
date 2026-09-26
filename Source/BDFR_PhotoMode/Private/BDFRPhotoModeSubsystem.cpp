#include "BDFRPhotoModeSubsystem.h"

#include "BDFRPhotoModePawn.h"
#include "BDFRPhotoModeSettings.h"
#include "BDFRPhotoPoseComponent.h"
#include "BDFRPhotoPosePreset.h"
#include "Engine/LocalPlayer.h"
#include "GameFramework/Actor.h"
#include "GameFramework/HUD.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Misc/Paths.h"
#include "UnrealClient.h"

APlayerController* UBDFRPhotoModeSubsystem::ResolvePlayerController() const
{
    ULocalPlayer* LocalPlayer = GetLocalPlayer();
    UWorld* World = LocalPlayer ? LocalPlayer->GetWorld() : nullptr;
    return (LocalPlayer && World) ? LocalPlayer->GetPlayerController(World) : nullptr;
}

bool UBDFRPhotoModeSubsystem::EnterPhotoMode()
{
    if (bPhotoModeActive)
    {
        return true;
    }

    APlayerController* PC = ResolvePlayerController();
    UWorld* World = PC ? PC->GetWorld() : nullptr;
    if (!PC || !World)
    {
        return false;
    }

    FVector ViewLocation;
    FRotator ViewRotation;
    PC->GetPlayerViewPoint(ViewLocation, ViewRotation);

    PreviousPawn = PC->GetPawn();
    PhotoSubject = PreviousPawn;
    PreviousHUD = PC->GetHUD();
    bWasPaused = UGameplayStatics::IsGamePaused(World);
    bPreviousShowMouseCursor = PC->bShowMouseCursor;

    if (AActor* Subject = PhotoSubject.Get())
    {
        SubjectRotationSnapshot = Subject->GetActorRotation();
        bHasSubjectRotationSnapshot = true;
    }

    if (AHUD* HUD = PreviousHUD.Get())
    {
        bPreviousHUDVisible = HUD->bShowHUD;
    }

    FActorSpawnParameters SpawnParams;
    SpawnParams.Owner = PC;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    ABDFRPhotoModePawn* SpawnedPawn = World->SpawnActor<ABDFRPhotoModePawn>(
        ABDFRPhotoModePawn::StaticClass(), ViewLocation, ViewRotation, SpawnParams);

    if (!SpawnedPawn)
    {
        return false;
    }

    PhotoPawn = SpawnedPawn;
    PC->Possess(SpawnedPawn);
    PC->bShowMouseCursor = false;

    const UBDFRPhotoModeSettings* Settings = GetDefault<UBDFRPhotoModeSettings>();
    if (Settings->bHideHUDOnEnter)
    {
        if (AHUD* HUD = PreviousHUD.Get())
        {
            HUD->bShowHUD = false;
        }
    }

    if (Settings->bFreezeWorldOnEnter && !bWasPaused)
    {
        UGameplayStatics::SetGamePaused(World, true);
    }

    bPhotoModeActive = true;
    OnPhotoModeEntered.Broadcast();
    return true;
}

void UBDFRPhotoModeSubsystem::ExitPhotoMode()
{
    if (!bPhotoModeActive)
    {
        return;
    }

    const UBDFRPhotoModeSettings* Settings = GetDefault<UBDFRPhotoModeSettings>();
    ClearPose();

    if (Settings->bRestoreSubjectRotationOnExit && bHasSubjectRotationSnapshot)
    {
        if (AActor* Subject = PhotoSubject.Get())
        {
            Subject->SetActorRotation(SubjectRotationSnapshot);
        }
    }

    ReleasePoseComponent();

    APlayerController* PC = ResolvePlayerController();
    UWorld* World = PC ? PC->GetWorld() : nullptr;

    if (World && !bWasPaused)
    {
        UGameplayStatics::SetGamePaused(World, false);
    }

    if (PC)
    {
        if (APawn* OldPawn = PreviousPawn.Get())
        {
            PC->Possess(OldPawn);
        }
        else
        {
            PC->UnPossess();
        }

        PC->bShowMouseCursor = bPreviousShowMouseCursor;
    }

    if (AHUD* HUD = PreviousHUD.Get())
    {
        HUD->bShowHUD = bPreviousHUDVisible;
    }

    if (ABDFRPhotoModePawn* Pawn = PhotoPawn.Get())
    {
        Pawn->Destroy();
    }

    PhotoPawn.Reset();
    PreviousPawn.Reset();
    PreviousHUD.Reset();
    PhotoSubject.Reset();
    bHasSubjectRotationSnapshot = false;
    bPhotoModeActive = false;

    OnPhotoModeExited.Broadcast();
}

bool UBDFRPhotoModeSubsystem::TogglePhotoMode()
{
    if (bPhotoModeActive)
    {
        ExitPhotoMode();
        return false;
    }

    return EnterPhotoMode();
}

bool UBDFRPhotoModeSubsystem::SetPhotoSubject(AActor* NewSubject)
{
    if (!NewSubject)
    {
        return false;
    }

    ClearPose();
    ReleasePoseComponent();
    PhotoSubject = NewSubject;
    SubjectRotationSnapshot = NewSubject->GetActorRotation();
    bHasSubjectRotationSnapshot = true;

    if (GetCameraMode() == EBDFRPhotoCameraMode::Selfie)
    {
        ConfigureSelfiePawn();
    }

    return true;
}

void UBDFRPhotoModeSubsystem::SetFOV(float NewFOV)
{
    if (ABDFRPhotoModePawn* Pawn = PhotoPawn.Get())
    {
        Pawn->SetPhotoFOV(NewFOV);
    }
}

bool UBDFRPhotoModeSubsystem::SetCameraMode(EBDFRPhotoCameraMode NewMode)
{
    ABDFRPhotoModePawn* Pawn = PhotoPawn.Get();
    if (!bPhotoModeActive || !Pawn)
    {
        return false;
    }

    if (NewMode == EBDFRPhotoCameraMode::Selfie)
    {
        if (!PhotoSubject.IsValid())
        {
            return false;
        }
        ConfigureSelfiePawn();
    }

    Pawn->SetCameraMode(NewMode);
    if (NewMode == EBDFRPhotoCameraMode::Selfie)
    {
        Pawn->SnapSelfieCamera();
    }
    OnCameraModeChanged.Broadcast(NewMode);
    return true;
}

EBDFRPhotoCameraMode UBDFRPhotoModeSubsystem::GetCameraMode() const
{
    if (const ABDFRPhotoModePawn* Pawn = PhotoPawn.Get())
    {
        return Pawn->GetCameraMode();
    }
    return EBDFRPhotoCameraMode::Free;
}

bool UBDFRPhotoModeSubsystem::EnterSelfieMode()
{
    const bool bSuccess = SetCameraMode(EBDFRPhotoCameraMode::Selfie);
    if (bSuccess && GetDefault<UBDFRPhotoModeSettings>()->bAutoLookAtCameraInSelfie)
    {
        LookSubjectAtCamera(true);
    }
    return bSuccess;
}

bool UBDFRPhotoModeSubsystem::ExitSelfieMode()
{
    return SetCameraMode(EBDFRPhotoCameraMode::Free);
}

void UBDFRPhotoModeSubsystem::ConfigureSelfiePawn()
{
    ABDFRPhotoModePawn* Pawn = PhotoPawn.Get();
    AActor* Subject = PhotoSubject.Get();
    if (!Pawn || !Subject)
    {
        return;
    }

    const UBDFRPhotoModeSettings* Settings = GetDefault<UBDFRPhotoModeSettings>();
    Pawn->ConfigureSelfie(
        Subject,
        Settings->SelfieDistance,
        Settings->SelfieHorizontalOffset,
        Settings->SelfieVerticalOffset,
        Settings->SelfieAnchorSocket,
        Settings->SelfieFOV,
        Settings->bSmoothSelfieCamera,
        Settings->SelfieSmoothingSpeed);
}

void UBDFRPhotoModeSubsystem::SetSelfieDistance(float NewDistance)
{
    if (ABDFRPhotoModePawn* Pawn = PhotoPawn.Get())
    {
        Pawn->SetSelfieDistance(NewDistance);
    }
}

void UBDFRPhotoModeSubsystem::SetSelfieOrbit(float YawOffset, float PitchOffset)
{
    if (ABDFRPhotoModePawn* Pawn = PhotoPawn.Get())
    {
        Pawn->SetSelfieOrbit(YawOffset, PitchOffset);
    }
}

UBDFRPhotoPoseComponent* UBDFRPhotoModeSubsystem::ResolvePoseComponent(bool bCreateIfMissing)
{
    if (PoseComponent)
    {
        return PoseComponent;
    }

    AActor* Subject = PhotoSubject.Get();
    if (!Subject)
    {
        return nullptr;
    }

    if (UBDFRPhotoPoseComponent* Existing = Subject->FindComponentByClass<UBDFRPhotoPoseComponent>())
    {
        PoseComponent = Existing;
        bOwnsPoseComponent = false;
        return PoseComponent;
    }

    if (!bCreateIfMissing)
    {
        return nullptr;
    }

    UBDFRPhotoPoseComponent* Created = NewObject<UBDFRPhotoPoseComponent>(Subject, NAME_None, RF_Transient);
    if (!Created)
    {
        return nullptr;
    }

    Subject->AddInstanceComponent(Created);
    Created->RegisterComponent();
    PoseComponent = Created;
    bOwnsPoseComponent = true;
    return PoseComponent;
}

void UBDFRPhotoModeSubsystem::ReleasePoseComponent()
{
    if (PoseComponent && bOwnsPoseComponent)
    {
        PoseComponent->DestroyComponent();
    }

    PoseComponent = nullptr;
    bOwnsPoseComponent = false;
}

UBDFRPhotoPosePreset* UBDFRPhotoModeSubsystem::FindPosePreset(FName PoseId) const
{
    if (PoseId.IsNone())
    {
        return nullptr;
    }

    const UBDFRPhotoModeSettings* Settings = GetDefault<UBDFRPhotoModeSettings>();
    for (const TSoftObjectPtr<UBDFRPhotoPosePreset>& PoseRef : Settings->PosePresets)
    {
        UBDFRPhotoPosePreset* Preset = PoseRef.LoadSynchronous();
        if (Preset && Preset->PoseId == PoseId)
        {
            return Preset;
        }
    }

    return nullptr;
}

bool UBDFRPhotoModeSubsystem::ApplyPosePreset(UBDFRPhotoPosePreset* Preset)
{
    if (!bPhotoModeActive || !Preset)
    {
        return false;
    }

    UBDFRPhotoPoseComponent* Component = ResolvePoseComponent(true);
    if (!Component || !Component->ApplyPose(Preset))
    {
        return false;
    }

    if (Preset->bLookAtCamera)
    {
        LookSubjectAtCamera(true);
    }

    OnPoseApplied.Broadcast(Preset->PoseId);
    return true;
}

bool UBDFRPhotoModeSubsystem::ApplyPoseById(FName PoseId)
{
    return ApplyPosePreset(FindPosePreset(PoseId));
}

void UBDFRPhotoModeSubsystem::ClearPose()
{
    if (UBDFRPhotoPoseComponent* Component = ResolvePoseComponent(false))
    {
        Component->ClearPose();
    }
}

TArray<FName> UBDFRPhotoModeSubsystem::GetAvailablePoseIds() const
{
    TArray<FName> Result;
    const UBDFRPhotoModeSettings* Settings = GetDefault<UBDFRPhotoModeSettings>();

    for (const TSoftObjectPtr<UBDFRPhotoPosePreset>& PoseRef : Settings->PosePresets)
    {
        if (UBDFRPhotoPosePreset* Preset = PoseRef.LoadSynchronous())
        {
            if (!Preset->PoseId.IsNone())
            {
                Result.AddUnique(Preset->PoseId);
            }
        }
    }

    return Result;
}

bool UBDFRPhotoModeSubsystem::LookSubjectAtCamera(bool bYawOnly)
{
    AActor* Subject = PhotoSubject.Get();
    ABDFRPhotoModePawn* Pawn = PhotoPawn.Get();
    if (!Subject || !Pawn)
    {
        return false;
    }

    const FVector ToCamera = Pawn->GetActorLocation() - Subject->GetActorLocation();
    if (ToCamera.IsNearlyZero())
    {
        return false;
    }

    FRotator TargetRotation = ToCamera.Rotation();
    if (bYawOnly)
    {
        TargetRotation.Pitch = Subject->GetActorRotation().Pitch;
        TargetRotation.Roll = Subject->GetActorRotation().Roll;
    }

    Subject->SetActorRotation(TargetRotation);
    return true;
}

bool UBDFRPhotoModeSubsystem::CaptureScreenshot(const FString& OptionalFilename, bool bShowUI)
{
    FString Filename = OptionalFilename;
    if (Filename.IsEmpty())
    {
        Filename = FPaths::Combine(FPaths::ProjectSavedDir(), TEXT("Screenshots"), TEXT("BDFR_PhotoMode.png"));
    }

    FScreenshotRequest::RequestScreenshot(Filename, bShowUI, true);
    return true;
}

void UBDFRPhotoModeSubsystem::Deinitialize()
{
    ExitPhotoMode();
    Super::Deinitialize();
}
