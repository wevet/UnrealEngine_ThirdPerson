// Copyright 2018 wevet works All Rights Reserved.

#include "AnimInstance/CharacterAnimInstanceBase.h"
#include "Kismet/KismetSystemLibrary.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PawnMovementComponent.h"

UCharacterAnimInstanceBase::UCharacterAnimInstanceBase(const FObjectInitializer& ObjectInitializer) 
	: Super(ObjectInitializer),
	bWasMoving(false),
	bWasAiming(false)
{
	Super::IKTargetInterpolationSpeed = 60.f;
	FalloutInterval = 3.f;
	bWasStanning = false;
}

void UCharacterAnimInstanceBase::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	OwningPawn = TryGetPawnOwner();
	Owner = Cast<ACharacterBase>(OwningPawn);

	if (Owner)
	{
		CharacterMovementComponent = Owner->GetCharacterMovement();
		CapsuleComponent = Owner->GetCapsuleComponent();
	}
}

void UCharacterAnimInstanceBase::NativeUpdateAnimation(float DeltaTimeX)
{
	Super::NativeUpdateAnimation(DeltaTimeX);

	if (!OwningPawn)
		return;


	SetMoving();
	if (CharacterMovementComponent)
	{
		IsFalling = CharacterMovementComponent->IsFalling();
	}

	if (IsFalling)
	{
		FalloutTickTime += DeltaTimeX;
		if (FalloutTickTime >= FalloutInterval)
		{
			IsFallout = true;
		}
	}
	else
	{
		IsFallout = false;
		FalloutTickTime = 0.f;
	}

	SetStanning();
	SetMovementSpeed();
	SetRotator();
	SetCrouch();
	SetEquip();
	SetWeaponItemType();
}

void UCharacterAnimInstanceBase::SetMoving()
{
	bWasMoving = (OwningPawn->GetVelocity().SizeSquared() > 25);
}

void UCharacterAnimInstanceBase::SetMovementSpeed()
{
	MovementSpeed = OwningPawn->GetVelocity().Size();
}

void UCharacterAnimInstanceBase::SetRotator()
{
	const FRotator ControlRotation = OwningPawn->GetControlRotation();
	const FRotator Rotation = OwningPawn->GetActorRotation();
	const FVector Velocity = OwningPawn->GetVelocity();
	CalcDirection = Super::CalculateDirection(Velocity, Rotation);

	const FRotator ResultRotation = NormalizedDeltaRotator(ControlRotation, Rotation);
	Yaw = ResultRotation.Yaw;
	Pitch = ResultRotation.Pitch;
}

void UCharacterAnimInstanceBase::SetStanning()
{
	if (Owner)
	{
		bWasStanning = Owner->WasStanning();
	}
}

void UCharacterAnimInstanceBase::SetCrouch()
{
	if (Owner)
	{
		IsCrouch = Owner->HasCrouch();
	}
}

void UCharacterAnimInstanceBase::SetEquip()
{
	if (Owner)
	{
		IsEquip = Owner->HasEquipWeapon();
	}
}

FRotator UCharacterAnimInstanceBase::NormalizedDeltaRotator(FRotator A, FRotator B) const
{
	FRotator Diff = A - B;
	Diff.Normalize();
	return Diff;
}

void UCharacterAnimInstanceBase::SetWeaponItemType()
{
	if (Owner)
	{
		WeaponItemType = Owner->GetCurrentWeaponType();
	}
}

bool UCharacterAnimInstanceBase::IsLocallyControlled() const
{
	if (TryGetPawnOwner())
	{
		return TryGetPawnOwner()->IsLocallyControlled();
	}
	return false;
}

// @NOTE
// PlayerˆÈŠO‚ÌCharacterBaseClass‚ÅŒÄ‚Î‚ê‚é
void UCharacterAnimInstanceBase::RagdollToWakeUpAction(const bool InFaceDown)
{
	Montage_Play(InFaceDown ? GetUpFromFront : GetUpFromBack, 1.0f, EMontagePlayReturnType::MontageLength, 0.0f, true);
}

#pragma region ALSInterface
void UCharacterAnimInstanceBase::SetWalkingSpeed_Implementation(const float InWalkingSpeed)
{
	WalkingSpeed = InWalkingSpeed;
	//UE_LOG(LogWevetClient, Log, TEXT("Walk : %s"), *FString(__FUNCTION__));
}

void UCharacterAnimInstanceBase::SetRunningSpeed_Implementation(const float InRunningSpeed)
{
	RunningSpeed = InRunningSpeed;
	//UE_LOG(LogWevetClient, Log, TEXT("Run : %s"), *FString(__FUNCTION__));
}

void UCharacterAnimInstanceBase::SetSprintingSpeed_Implementation(const float InSprintingSpeed)
{
	SprintingSpeed = InSprintingSpeed;
	//UE_LOG(LogWevetClient, Log, TEXT("Sprint : %s"), *FString(__FUNCTION__));
}

void UCharacterAnimInstanceBase::SetCrouchingSpeed_Implementation(const float InCrouchingSpeed)
{
	CrouchingSpeed = InCrouchingSpeed;
	//UE_LOG(LogWevetClient, Log, TEXT("Crouch : %s"), *FString(__FUNCTION__));
}

void UCharacterAnimInstanceBase::SetSwimmingSpeed_Implementation(const float InSwimmingSpeed)
{
	SwimmingSpeed = InSwimmingSpeed;
	//UE_LOG(LogWevetClient, Log, TEXT("Swim : %s"), *FString(__FUNCTION__));
}
#pragma endregion
