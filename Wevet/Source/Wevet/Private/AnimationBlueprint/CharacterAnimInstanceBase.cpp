// Copyright 2018 wevet works All Rights Reserved.

#include "CharacterAnimInstanceBase.h"
#include "Kismet/KismetSystemLibrary.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PawnMovementComponent.h"

UCharacterAnimInstanceBase::UCharacterAnimInstanceBase(const FObjectInitializer& ObjectInitializer) 
	: Super(ObjectInitializer),
	MaxBlendWeight(0.8f)
{

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
	IsMoving = (OwningPawn->GetVelocity().SizeSquared() > 25);
	Speed = OwningPawn->GetVelocity().Size();

	if (UPawnMovementComponent* MovementComponent = OwningPawn->GetMovementComponent())
	{
		IsFalling = MovementComponent->IsFalling();
		//const bool isInAir = MovementComponent->MovementMode == EMovementMode::MOVE_Falling;
		//const float Velocity = MovementComponent->GetVelocity();
	}

	SetRotator();
	SetCrouch();
	SetEquip();
	BlendWeight = (IsEquip) ? MaxBlendWeight : 0.f;
}

void UCharacterAnimInstanceBase::SetRotator()
{
	const FRotator ControlRotation = OwningPawn->GetControlRotation();
	const FRotator Rotation = OwningPawn->GetActorRotation();
	const FVector Velocity = OwningPawn->GetVelocity();
	Direction = Super::CalculateDirection(Velocity, Rotation);

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
		//IsHanging = Owner->HasHanging();
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