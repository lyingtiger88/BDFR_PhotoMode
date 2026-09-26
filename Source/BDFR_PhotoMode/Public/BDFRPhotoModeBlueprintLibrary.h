#pragma once

#include "CoreMinimal.h"
#include "BDFRPhotoModeTypes.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "BDFRPhotoModeBlueprintLibrary.generated.h"

class AActor;
class UBDFRPhotoModeSubsystem;
class UBDFRPhotoPosePreset;

UCLASS()
class BDFR_PHOTOMODE_API UBDFRPhotoModeBlueprintLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintPure, Category="BDFR|Photo Mode", meta=(WorldContext="WorldContextObject"))
    static UBDFRPhotoModeSubsystem* GetPhotoModeSubsystem(const UObject* WorldContextObject, int32 PlayerIndex = 0);

    UFUNCTION(BlueprintCallable, Category="BDFR|Photo Mode", meta=(WorldContext="WorldContextObject"))
    static bool TogglePhotoMode(const UObject* WorldContextObject, int32 PlayerIndex = 0);

    UFUNCTION(BlueprintCallable, Category="BDFR|Photo Mode", meta=(WorldContext="WorldContextObject"))
    static bool EnterPhotoMode(const UObject* WorldContextObject, int32 PlayerIndex = 0);

    UFUNCTION(BlueprintCallable, Category="BDFR|Photo Mode", meta=(WorldContext="WorldContextObject"))
    static void ExitPhotoMode(const UObject* WorldContextObject, int32 PlayerIndex = 0);

    UFUNCTION(BlueprintCallable, Category="BDFR|Photo Mode|Selfie", meta=(WorldContext="WorldContextObject"))
    static bool EnterSelfieMode(const UObject* WorldContextObject, int32 PlayerIndex = 0);

    UFUNCTION(BlueprintCallable, Category="BDFR|Photo Mode|Selfie", meta=(WorldContext="WorldContextObject"))
    static bool ExitSelfieMode(const UObject* WorldContextObject, int32 PlayerIndex = 0);

    UFUNCTION(BlueprintCallable, Category="BDFR|Photo Mode|Pose", meta=(WorldContext="WorldContextObject"))
    static bool ApplyPoseById(const UObject* WorldContextObject, FName PoseId, int32 PlayerIndex = 0);

    UFUNCTION(BlueprintCallable, Category="BDFR|Photo Mode|Pose", meta=(WorldContext="WorldContextObject"))
    static bool ApplyPosePreset(const UObject* WorldContextObject, UBDFRPhotoPosePreset* Preset, int32 PlayerIndex = 0);

    UFUNCTION(BlueprintCallable, Category="BDFR|Photo Mode|Pose", meta=(WorldContext="WorldContextObject"))
    static void ClearPose(const UObject* WorldContextObject, int32 PlayerIndex = 0);

    UFUNCTION(BlueprintCallable, Category="BDFR|Photo Mode|Subject", meta=(WorldContext="WorldContextObject"))
    static bool SetPhotoSubject(const UObject* WorldContextObject, AActor* Subject, int32 PlayerIndex = 0);
};
