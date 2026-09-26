#pragma once

#include "CoreMinimal.h"
#include "Engine/PrimaryDataAsset.h"
#include "BDFRPhotoPosePreset.generated.h"

class UAnimMontage;
class UAnimSequenceBase;

UCLASS(BlueprintType)
class BDFR_PHOTOMODE_API UBDFRPhotoPosePreset : public UPrimaryDataAsset
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Pose|Identity")
    FName PoseId = NAME_None;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Pose|Identity")
    FText DisplayName;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Pose|Identity")
    FName Category = TEXT("General");

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Pose|Animation")
    TSoftObjectPtr<UAnimMontage> Montage;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Pose|Animation")
    TSoftObjectPtr<UAnimSequenceBase> Animation;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Pose|Animation")
    FName SlotName = TEXT("DefaultSlot");

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Pose|Animation", meta=(ClampMin="0.01"))
    float PlayRate = 1.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Pose|Animation")
    bool bLoop = false;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Pose|Animation", meta=(ClampMin="0.0"))
    float StartTime = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Pose|Animation")
    bool bAnimateWhileWorldPaused = false;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Pose|Animation", meta=(ClampMin="0.0"))
    float BlendInTime = 0.2f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Pose|Animation", meta=(ClampMin="0.0"))
    float BlendOutTime = 0.2f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Pose|Photo Mode")
    bool bLookAtCamera = false;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Pose|Photo Mode")
    bool bRecommendedForSelfie = false;
};
