#include "BDFRPhotoPoseComponent.h"

#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "Animation/AnimSequenceBase.h"
#include "BDFRPhotoPosePreset.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/Actor.h"
#include "Kismet/GameplayStatics.h"
#include "HAL/PlatformTime.h"

UBDFRPhotoPoseComponent::UBDFRPhotoPoseComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
    PrimaryComponentTick.bStartWithTickEnabled = true;
    PrimaryComponentTick.bTickEvenWhenPaused = true;
}

void UBDFRPhotoPoseComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    if (!ActiveMontage || !ActivePreset || !ActivePreset->bAnimateWhileWorldPaused)
    {
        LastRealTimeSeconds = FPlatformTime::Seconds();
        return;
    }

    UWorld* World = GetWorld();
    if (!World || !UGameplayStatics::IsGamePaused(World))
    {
        LastRealTimeSeconds = FPlatformTime::Seconds();
        return;
    }

    USkeletalMeshComponent* Mesh = ResolveMesh();
    UAnimInstance* AnimInstance = Mesh ? Mesh->GetAnimInstance() : nullptr;
    if (!AnimInstance)
    {
        return;
    }

    const double Now = FPlatformTime::Seconds();
    if (LastRealTimeSeconds <= 0.0)
    {
        LastRealTimeSeconds = Now;
        return;
    }

    const float RealDelta = static_cast<float>(FMath::Clamp(Now - LastRealTimeSeconds, 0.0, 0.1));
    LastRealTimeSeconds = Now;

    const float Length = ActiveMontage->GetPlayLength();
    if (Length <= KINDA_SMALL_NUMBER)
    {
        return;
    }

    float NewPosition = AnimInstance->Montage_GetPosition(ActiveMontage) + RealDelta * ActivePreset->PlayRate;
    if (NewPosition >= Length)
    {
        if (ActivePreset->bLoop)
        {
            NewPosition = FMath::Fmod(NewPosition, Length);
        }
        else
        {
            NewPosition = FMath::Max(0.0f, Length - KINDA_SMALL_NUMBER);
        }
    }

    AnimInstance->Montage_SetPosition(ActiveMontage, NewPosition);
}

USkeletalMeshComponent* UBDFRPhotoPoseComponent::ResolveMesh()
{
    if (TargetMesh.IsValid())
    {
        return TargetMesh.Get();
    }

    AActor* Owner = GetOwner();
    USkeletalMeshComponent* Mesh = Owner ? Owner->FindComponentByClass<USkeletalMeshComponent>() : nullptr;
    TargetMesh = Mesh;
    return Mesh;
}

void UBDFRPhotoPoseComponent::SetTargetMesh(USkeletalMeshComponent* NewMesh)
{
    TargetMesh = NewMesh;
}

bool UBDFRPhotoPoseComponent::ApplyPose(UBDFRPhotoPosePreset* Preset)
{
    if (!Preset)
    {
        return false;
    }

    USkeletalMeshComponent* Mesh = ResolveMesh();
    UAnimInstance* AnimInstance = Mesh ? Mesh->GetAnimInstance() : nullptr;
    if (!Mesh || !AnimInstance)
    {
        return false;
    }

    ClearPose(Preset->BlendOutTime);

    if (!bHasSavedTickPolicy)
    {
        bSavedTickEvenWhenPaused = Mesh->PrimaryComponentTick.bTickEvenWhenPaused;
        bHasSavedTickPolicy = true;
    }
    Mesh->PrimaryComponentTick.bTickEvenWhenPaused = true;
    Mesh->SetComponentTickEnabled(true);

    UAnimMontage* MontageToPlay = Preset->Montage.LoadSynchronous();
    if (MontageToPlay)
    {
        const float PlayedLength = AnimInstance->Montage_Play(
            MontageToPlay,
            Preset->PlayRate,
            EMontagePlayReturnType::MontageLength,
            Preset->StartTime,
            true);

        if (PlayedLength <= 0.0f)
        {
            return false;
        }

        ActiveMontage = MontageToPlay;
    }
    else if (UAnimSequenceBase* Sequence = Preset->Animation.LoadSynchronous())
    {
        const int32 LoopCount = Preset->bLoop ? 1000000 : 1;
        ActiveMontage = AnimInstance->PlaySlotAnimationAsDynamicMontage(
            Sequence,
            Preset->SlotName,
            Preset->BlendInTime,
            Preset->BlendOutTime,
            Preset->PlayRate,
            LoopCount,
            -1.0f,
            Preset->StartTime);

        if (!ActiveMontage)
        {
            return false;
        }
    }
    else
    {
        return false;
    }

    ActivePreset = Preset;
    ActivePoseId = Preset->PoseId;
    ActiveBlendOutTime = Preset->BlendOutTime;
    LastRealTimeSeconds = FPlatformTime::Seconds();
    OnPoseChanged.Broadcast(ActivePoseId);
    return true;
}

void UBDFRPhotoPoseComponent::ClearPose(float OverrideBlendOutTime)
{
    USkeletalMeshComponent* Mesh = ResolveMesh();
    UAnimInstance* AnimInstance = Mesh ? Mesh->GetAnimInstance() : nullptr;

    if (AnimInstance && ActiveMontage)
    {
        const float BlendOut = OverrideBlendOutTime >= 0.0f ? OverrideBlendOutTime : ActiveBlendOutTime;
        AnimInstance->Montage_Stop(BlendOut, ActiveMontage);
    }

    ActiveMontage = nullptr;
    ActivePreset = nullptr;
    ActivePoseId = NAME_None;

    if (Mesh && bHasSavedTickPolicy)
    {
        Mesh->PrimaryComponentTick.bTickEvenWhenPaused = bSavedTickEvenWhenPaused;
    }

    bHasSavedTickPolicy = false;
    LastRealTimeSeconds = 0.0;
    OnPoseChanged.Broadcast(NAME_None);
}

void UBDFRPhotoPoseComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    ClearPose(0.0f);
    Super::EndPlay(EndPlayReason);
}
