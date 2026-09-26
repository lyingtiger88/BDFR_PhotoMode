#include "BDFRPhotoModePawn.h"

#include "BDFRPhotoModeSettings.h"
#include "Camera/CameraComponent.h"
#include "Components/SceneComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/Actor.h"
#include "GameFramework/PlayerController.h"
#include "InputCoreTypes.h"

ABDFRPhotoModePawn::ABDFRPhotoModePawn()
{
    PrimaryActorTick.bCanEverTick = true;
    PrimaryActorTick.bTickEvenWhenPaused = true;
    bUseControllerRotationPitch = false;
    bUseControllerRotationYaw = false;
    bUseControllerRotationRoll = false;

    SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
    SetRootComponent(SceneRoot);

    Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("PhotoCamera"));
    Camera->SetupAttachment(SceneRoot);
    Camera->bUsePawnControlRotation = false;

    const UBDFRPhotoModeSettings* Settings = GetDefault<UBDFRPhotoModeSettings>();
    CurrentMoveSpeed = Settings->MoveSpeed;
    LookSensitivity = Settings->LookSensitivity;
    FastMoveMultiplier = Settings->FastMoveMultiplier;
    SpeedStep = Settings->SpeedStep;
    MinFOV = Settings->MinFOV;
    MaxFOV = Settings->MaxFOV;
    Camera->SetFieldOfView(FMath::Clamp(Settings->DefaultFOV, MinFOV, MaxFOV));

    MinSelfieDistance = Settings->MinSelfieDistance;
    MaxSelfieDistance = Settings->MaxSelfieDistance;

    AutoPossessAI = EAutoPossessAI::Disabled;
    SetActorEnableCollision(false);
}

void ABDFRPhotoModePawn::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    APlayerController* PC = Cast<APlayerController>(GetController());
    if (!PC)
    {
        return;
    }

    const float EffectiveDelta = DeltaSeconds > KINDA_SMALL_NUMBER ? DeltaSeconds : (1.0f / 60.0f);

    if (CameraMode == EBDFRPhotoCameraMode::Selfie)
    {
        TickSelfieCamera(PC, EffectiveDelta);
    }
    else
    {
        TickFreeCamera(PC, EffectiveDelta);
    }
}

void ABDFRPhotoModePawn::TickFreeCamera(APlayerController* PC, float EffectiveDelta)
{
    if (PC->IsInputKeyDown(EKeys::RightMouseButton))
    {
        float MouseX = 0.0f;
        float MouseY = 0.0f;
        PC->GetInputMouseDelta(MouseX, MouseY);

        FRotator Rotation = GetActorRotation();
        Rotation.Yaw += MouseX * LookSensitivity;
        Rotation.Pitch = FMath::ClampAngle(Rotation.Pitch - MouseY * LookSensitivity, -89.0f, 89.0f);
        Rotation.Roll = 0.0f;
        SetActorRotation(Rotation);
    }

    FVector MoveInput = FVector::ZeroVector;
    MoveInput.X += PC->IsInputKeyDown(EKeys::W) ? 1.0f : 0.0f;
    MoveInput.X -= PC->IsInputKeyDown(EKeys::S) ? 1.0f : 0.0f;
    MoveInput.Y += PC->IsInputKeyDown(EKeys::D) ? 1.0f : 0.0f;
    MoveInput.Y -= PC->IsInputKeyDown(EKeys::A) ? 1.0f : 0.0f;
    MoveInput.Z += PC->IsInputKeyDown(EKeys::E) ? 1.0f : 0.0f;
    MoveInput.Z -= PC->IsInputKeyDown(EKeys::Q) ? 1.0f : 0.0f;

    if (!MoveInput.IsNearlyZero())
    {
        MoveInput.Normalize();
        const bool bFast = PC->IsInputKeyDown(EKeys::LeftShift) || PC->IsInputKeyDown(EKeys::RightShift);
        const float FinalSpeed = CurrentMoveSpeed * (bFast ? FastMoveMultiplier : 1.0f);

        const FVector WorldMove =
            GetActorForwardVector() * MoveInput.X +
            GetActorRightVector() * MoveInput.Y +
            FVector::UpVector * MoveInput.Z;

        AddActorWorldOffset(WorldMove * FinalSpeed * EffectiveDelta, false);
    }

    if (PC->WasInputKeyJustPressed(EKeys::MouseScrollUp))
    {
        CurrentMoveSpeed += SpeedStep;
    }
    else if (PC->WasInputKeyJustPressed(EKeys::MouseScrollDown))
    {
        CurrentMoveSpeed = FMath::Max(25.0f, CurrentMoveSpeed - SpeedStep);
    }
}

void ABDFRPhotoModePawn::TickSelfieCamera(APlayerController* PC, float EffectiveDelta)
{
    if (!SelfieSubject.IsValid())
    {
        return;
    }

    if (PC->IsInputKeyDown(EKeys::RightMouseButton))
    {
        float MouseX = 0.0f;
        float MouseY = 0.0f;
        PC->GetInputMouseDelta(MouseX, MouseY);
        SelfieYawOffset = FMath::Clamp(SelfieYawOffset + MouseX * LookSensitivity, -65.0f, 65.0f);
        SelfiePitchOffset = FMath::Clamp(SelfiePitchOffset - MouseY * LookSensitivity, -45.0f, 45.0f);
    }

    if (PC->WasInputKeyJustPressed(EKeys::MouseScrollUp))
    {
        SetSelfieDistance(SelfieDistance - 7.5f);
    }
    else if (PC->WasInputKeyJustPressed(EKeys::MouseScrollDown))
    {
        SetSelfieDistance(SelfieDistance + 7.5f);
    }

    const FVector Anchor = ResolveSelfieAnchor();
    const FRotator SubjectYaw(0.0f, SelfieSubject->GetActorRotation().Yaw, 0.0f);
    const FRotator OrbitRotation(SelfiePitchOffset, SubjectYaw.Yaw + SelfieYawOffset, 0.0f);
    const FVector Forward = OrbitRotation.Vector();
    const FVector Right = FRotationMatrix(SubjectYaw).GetScaledAxis(EAxis::Y);

    const FVector DesiredLocation = Anchor
        + Forward * SelfieDistance
        + Right * SelfieHorizontalOffset
        + FVector::UpVector * SelfieVerticalOffset;

    const FRotator DesiredRotation = (Anchor - DesiredLocation).Rotation();

    if (bSmoothSelfieCamera)
    {
        SetActorLocation(FMath::VInterpTo(GetActorLocation(), DesiredLocation, EffectiveDelta, SelfieSmoothingSpeed));
        SetActorRotation(FMath::RInterpTo(GetActorRotation(), DesiredRotation, EffectiveDelta, SelfieSmoothingSpeed));
    }
    else
    {
        SetActorLocationAndRotation(DesiredLocation, DesiredRotation);
    }
}

FVector ABDFRPhotoModePawn::ResolveSelfieAnchor() const
{
    AActor* Subject = SelfieSubject.Get();
    if (!Subject)
    {
        return GetActorLocation();
    }

    if (USkeletalMeshComponent* Mesh = Subject->FindComponentByClass<USkeletalMeshComponent>())
    {
        if (!SelfieAnchorSocket.IsNone() && Mesh->DoesSocketExist(SelfieAnchorSocket))
        {
            return Mesh->GetSocketLocation(SelfieAnchorSocket);
        }
    }

    return Subject->GetActorLocation() + FVector(0.0f, 0.0f, 70.0f);
}

void ABDFRPhotoModePawn::SetPhotoFOV(float NewFOV)
{
    if (Camera)
    {
        Camera->SetFieldOfView(FMath::Clamp(NewFOV, MinFOV, MaxFOV));
    }
}

float ABDFRPhotoModePawn::GetPhotoFOV() const
{
    return Camera ? Camera->FieldOfView : 0.0f;
}

void ABDFRPhotoModePawn::SetMoveSpeed(float NewSpeed)
{
    CurrentMoveSpeed = FMath::Max(25.0f, NewSpeed);
}

void ABDFRPhotoModePawn::SetCameraMode(EBDFRPhotoCameraMode NewMode)
{
    if (CameraMode == NewMode)
    {
        return;
    }

    if (NewMode == EBDFRPhotoCameraMode::Selfie)
    {
        PreSelfieFOV = GetPhotoFOV();
        SetPhotoFOV(SelfieFOV);
    }
    else if (CameraMode == EBDFRPhotoCameraMode::Selfie)
    {
        SetPhotoFOV(PreSelfieFOV);
    }

    CameraMode = NewMode;
}

void ABDFRPhotoModePawn::ConfigureSelfie(AActor* InSubject, float InDistance, float InHorizontalOffset, float InVerticalOffset, FName InAnchorSocket, float InSelfieFOV, bool bInSmooth, float InSmoothingSpeed)
{
    SelfieSubject = InSubject;
    SelfieDistance = FMath::Clamp(InDistance, MinSelfieDistance, MaxSelfieDistance);
    SelfieHorizontalOffset = InHorizontalOffset;
    SelfieVerticalOffset = InVerticalOffset;
    SelfieAnchorSocket = InAnchorSocket;
    SelfieFOV = InSelfieFOV;
    bSmoothSelfieCamera = bInSmooth;
    SelfieSmoothingSpeed = FMath::Max(0.1f, InSmoothingSpeed);
    SelfieYawOffset = 0.0f;
    SelfiePitchOffset = 0.0f;
}

void ABDFRPhotoModePawn::SetSelfieDistance(float NewDistance)
{
    SelfieDistance = FMath::Clamp(NewDistance, MinSelfieDistance, MaxSelfieDistance);
}

void ABDFRPhotoModePawn::SetSelfieOrbit(float YawOffset, float PitchOffset)
{
    SelfieYawOffset = FMath::Clamp(YawOffset, -65.0f, 65.0f);
    SelfiePitchOffset = FMath::Clamp(PitchOffset, -45.0f, 45.0f);
}

void ABDFRPhotoModePawn::SnapSelfieCamera()
{
    if (!SelfieSubject.IsValid())
    {
        return;
    }

    const FVector Anchor = ResolveSelfieAnchor();
    const FRotator SubjectYaw(0.0f, SelfieSubject->GetActorRotation().Yaw, 0.0f);
    const FRotator OrbitRotation(SelfiePitchOffset, SubjectYaw.Yaw + SelfieYawOffset, 0.0f);
    const FVector Forward = OrbitRotation.Vector();
    const FVector Right = FRotationMatrix(SubjectYaw).GetScaledAxis(EAxis::Y);

    const FVector DesiredLocation = Anchor
        + Forward * SelfieDistance
        + Right * SelfieHorizontalOffset
        + FVector::UpVector * SelfieVerticalOffset;

    SetActorLocationAndRotation(DesiredLocation, (Anchor - DesiredLocation).Rotation());
}
