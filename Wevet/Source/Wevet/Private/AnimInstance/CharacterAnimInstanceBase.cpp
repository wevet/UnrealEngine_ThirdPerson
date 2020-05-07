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
	CombatBlendWeight = 0.8f;
	ClimbBlendWeight  = 1.f;
	BlendWeight = 0.f;
	FalloutInterval = 3.f;
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

	if (OwningPawn == nullptr)
	{
		return;
	}

	bHasMoving = (OwningPawn->GetVelocity().SizeSquared() > 25);

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

	SetMovementSpeed();
	SetRotator();
	SetCrouch();
	SetEquip();
	SetHanging();
	SetClimbingLedge();
	SetClimbingMove();
	SetWeaponItemType();
	BlendWeight = (IsEquip) ? FMath::Clamp<float>(CombatBlendWeight, 0.f, 1.f) : 0.f;
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

void UCharacterAnimInstanceBase::SetHanging()
{
	if (Owner)
	{
		IsHanging = Owner->HasHanging();
	}
}

void UCharacterAnimInstanceBase::SetClimbingLedge()
{
	if (Owner)
	{
		IsClimbingLedge = Owner->HasClimbingLedge();
	}
}

void UCharacterAnimInstanceBase::SetClimbingMove()
{
	if (Owner)
	{
		bCanClimbMoveLeft  = Owner->HasClimbingMoveLeft();
		bCanClimbMoveRight = Owner->HasClimbingMoveRight();
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

#pragma region IGrabInstigator
void UCharacterAnimInstanceBase::CanGrab_Implementation(bool InCanGrab)
{
}

void UCharacterAnimInstanceBase::ClimbLedge_Implementation(bool InClimbLedge)
{
}

void UCharacterAnimInstanceBase::ReportClimbEnd_Implementation()
{
}

void UCharacterAnimInstanceBase::ClimbMove_Implementation(float Value)
{
	IsClimbMoveRight = (Value > 0.f);
	IsClimbMoveLeft  = (Value < 0.f);
}

void UCharacterAnimInstanceBase::ClimbJump_Implementation()
{
	bClimbJumping = true;
}

void UCharacterAnimInstanceBase::ReportClimbJumpEnd_Implementation()
{
	//if (Owner)
	//{
	//	IGrabExecuter::Execute_ReportClimbJumpEnd(Owner);
	//}
	bClimbJumping = false;
}

void UCharacterAnimInstanceBase::TurnConerLeftUpdate_Implementation()
{

}

void UCharacterAnimInstanceBase::TurnConerRightUpdate_Implementation()
{

}

void UCharacterAnimInstanceBase::TurnConerResult_Implementation()
{

}
#pragma endregion

#pragma region ALSInterface
void UCharacterAnimInstanceBase::SetWalkingSpeed_Implementation(const float InWalkingSpeed)
{
	WalkingSpeed = InWalkingSpeed;
	UE_LOG(LogWevetClient, Log, TEXT("Walk : %s"), *FString(__FUNCTION__));
}

void UCharacterAnimInstanceBase::SetRunningSpeed_Implementation(const float InRunningSpeed)
{
	RunningSpeed = InRunningSpeed;
	UE_LOG(LogWevetClient, Log, TEXT("Run : %s"), *FString(__FUNCTION__));
}

void UCharacterAnimInstanceBase::SetSprintingSpeed_Implementation(const float InSprintingSpeed)
{
	SprintingSpeed = InSprintingSpeed;
	UE_LOG(LogWevetClient, Log, TEXT("Sprint : %s"), *FString(__FUNCTION__));
}

void UCharacterAnimInstanceBase::SetCrouchingSpeed_Implementation(const float InCrouchingSpeed)
{
	CrouchingSpeed = InCrouchingSpeed;
	UE_LOG(LogWevetClient, Log, TEXT("Crouch : %s"), *FString(__FUNCTION__));
}
#pragma endregion
