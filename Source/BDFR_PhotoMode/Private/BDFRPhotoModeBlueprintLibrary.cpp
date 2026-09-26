#include "BDFRPhotoModeBlueprintLibrary.h"

#include "BDFRPhotoModeSubsystem.h"
#include "Engine/LocalPlayer.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"

UBDFRPhotoModeSubsystem* UBDFRPhotoModeBlueprintLibrary::GetPhotoModeSubsystem(const UObject* WorldContextObject, int32 PlayerIndex)
{
    if (!WorldContextObject)
    {
        return nullptr;
    }

    APlayerController* PC = UGameplayStatics::GetPlayerController(WorldContextObject, PlayerIndex);
    ULocalPlayer* LocalPlayer = PC ? PC->GetLocalPlayer() : nullptr;
    return LocalPlayer ? LocalPlayer->GetSubsystem<UBDFRPhotoModeSubsystem>() : nullptr;
}

bool UBDFRPhotoModeBlueprintLibrary::TogglePhotoMode(const UObject* WorldContextObject, int32 PlayerIndex)
{
    if (UBDFRPhotoModeSubsystem* Subsystem = GetPhotoModeSubsystem(WorldContextObject, PlayerIndex))
    {
        return Subsystem->TogglePhotoMode();
    }
    return false;
}

bool UBDFRPhotoModeBlueprintLibrary::EnterPhotoMode(const UObject* WorldContextObject, int32 PlayerIndex)
{
    if (UBDFRPhotoModeSubsystem* Subsystem = GetPhotoModeSubsystem(WorldContextObject, PlayerIndex))
    {
        return Subsystem->EnterPhotoMode();
    }
    return false;
}

void UBDFRPhotoModeBlueprintLibrary::ExitPhotoMode(const UObject* WorldContextObject, int32 PlayerIndex)
{
    if (UBDFRPhotoModeSubsystem* Subsystem = GetPhotoModeSubsystem(WorldContextObject, PlayerIndex))
    {
        Subsystem->ExitPhotoMode();
    }
}

bool UBDFRPhotoModeBlueprintLibrary::EnterSelfieMode(const UObject* WorldContextObject, int32 PlayerIndex)
{
    if (UBDFRPhotoModeSubsystem* Subsystem = GetPhotoModeSubsystem(WorldContextObject, PlayerIndex))
    {
        return Subsystem->EnterSelfieMode();
    }
    return false;
}

bool UBDFRPhotoModeBlueprintLibrary::ExitSelfieMode(const UObject* WorldContextObject, int32 PlayerIndex)
{
    if (UBDFRPhotoModeSubsystem* Subsystem = GetPhotoModeSubsystem(WorldContextObject, PlayerIndex))
    {
        return Subsystem->ExitSelfieMode();
    }
    return false;
}

bool UBDFRPhotoModeBlueprintLibrary::ApplyPoseById(const UObject* WorldContextObject, FName PoseId, int32 PlayerIndex)
{
    if (UBDFRPhotoModeSubsystem* Subsystem = GetPhotoModeSubsystem(WorldContextObject, PlayerIndex))
    {
        return Subsystem->ApplyPoseById(PoseId);
    }
    return false;
}

bool UBDFRPhotoModeBlueprintLibrary::ApplyPosePreset(const UObject* WorldContextObject, UBDFRPhotoPosePreset* Preset, int32 PlayerIndex)
{
    if (UBDFRPhotoModeSubsystem* Subsystem = GetPhotoModeSubsystem(WorldContextObject, PlayerIndex))
    {
        return Subsystem->ApplyPosePreset(Preset);
    }
    return false;
}

void UBDFRPhotoModeBlueprintLibrary::ClearPose(const UObject* WorldContextObject, int32 PlayerIndex)
{
    if (UBDFRPhotoModeSubsystem* Subsystem = GetPhotoModeSubsystem(WorldContextObject, PlayerIndex))
    {
        Subsystem->ClearPose();
    }
}

bool UBDFRPhotoModeBlueprintLibrary::SetPhotoSubject(const UObject* WorldContextObject, AActor* Subject, int32 PlayerIndex)
{
    if (UBDFRPhotoModeSubsystem* Subsystem = GetPhotoModeSubsystem(WorldContextObject, PlayerIndex))
    {
        return Subsystem->SetPhotoSubject(Subject);
    }
    return false;
}
