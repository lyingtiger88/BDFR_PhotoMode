#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BDFRPhotoPoseComponent.generated.h"

class UAnimMontage;
class USkeletalMeshComponent;
class UBDFRPhotoPosePreset;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FBDFRPhotoPoseChanged, FName, PoseId);

UCLASS(ClassGroup=(BDFR), meta=(BlueprintSpawnableComponent))
class BDFR_PHOTOMODE_API UBDFRPhotoPoseComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UBDFRPhotoPoseComponent();

    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

    UFUNCTION(BlueprintCallable, Category="BDFR|Photo Mode|Pose")
    bool ApplyPose(UBDFRPhotoPosePreset* Preset);

    UFUNCTION(BlueprintCallable, Category="BDFR|Photo Mode|Pose")
    void ClearPose(float OverrideBlendOutTime = -1.0f);

    UFUNCTION(BlueprintPure, Category="BDFR|Photo Mode|Pose")
    FName GetActivePoseId() const { return ActivePoseId; }

    UFUNCTION(BlueprintPure, Category="BDFR|Photo Mode|Pose")
    UBDFRPhotoPosePreset* GetActivePreset() const { return ActivePreset; }

    UFUNCTION(BlueprintCallable, Category="BDFR|Photo Mode|Pose")
    void SetTargetMesh(USkeletalMeshComponent* NewMesh);

    UPROPERTY(BlueprintAssignable, Category="BDFR|Photo Mode|Pose")
    FBDFRPhotoPoseChanged OnPoseChanged;

protected:
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
    USkeletalMeshComponent* ResolveMesh();

    UPROPERTY(Transient)
    TWeakObjectPtr<USkeletalMeshComponent> TargetMesh;

    UPROPERTY(Transient)
    TObjectPtr<UAnimMontage> ActiveMontage = nullptr;

    UPROPERTY(Transient)
    TObjectPtr<UBDFRPhotoPosePreset> ActivePreset = nullptr;

    FName ActivePoseId = NAME_None;
    bool bSavedTickEvenWhenPaused = false;
    bool bHasSavedTickPolicy = false;
    float ActiveBlendOutTime = 0.2f;
    double LastRealTimeSeconds = 0.0;
};
