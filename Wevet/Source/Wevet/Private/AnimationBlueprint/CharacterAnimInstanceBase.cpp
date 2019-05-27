// Copyright 2018 wevet works All Rights Reserved.

#include "CharacterAnimInstanceBase.h"
#include "Kismet/KismetSystemLibrary.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PawnMovementComponent.h"

UCharacterAnimInstanceBase::UCharacterAnimInstanceBase(const FObjectInitializer& ObjectInitializer) 
	: Super(ObjectInitializer)
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
}

void UCharacterAnimInstanceBase::NativeUpdateAnimation(float DeltaTimeX)
{
	Super::NativeUpdateAnimation(DeltaTimeX);

	if (OwningPawn == nullptr)
	{
		return;
	}

	Owner = Cast<ACharacterBase>(OwningPawn);
	bHasMoving = (OwningPawn->GetVelocity().SizeSquared() > 25);
	MovementSpeed = OwningPawn->GetVelocity().Size();

	if (UPawnMovementComponent* MovementComponent = OwningPawn->GetMovementComponent())
	{
		IsFalling = MovementComponent->IsFalling();
		//const bool isInAir = MovementComponent->MovementMode == EMovementMode::MOVE_Falling;
		//const float Velocity = MovementComponent->GetVelocity();
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

	SetRotator();
	SetCrouch();
	SetEquip();
	SetHanging();
	SetClimbingLedge();
	SetClimbingMove();
	BlendWeight = (IsEquip) ? FMath::Clamp<float>(CombatBlendWeight, 0.f, 1.f) : 0.f;
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
