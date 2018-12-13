// Copyright 2018 wevet works All Rights Reserved.

#include "CharacterAnimInstanceBase.h"
#include "Kismet/KismetSystemLibrary.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PawnMovementComponent.h"

UCharacterAnimInstanceBase::UCharacterAnimInstanceBase(const FObjectInitializer& ObjectInitializer) 
	: Super(ObjectInitializer)
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

	UPawnMovementComponent* MovementComponent = OwningPawn->GetMovementComponent();
	
	if (MovementComponent)
	{
		IsFalling = MovementComponent->IsFalling();
		//const bool isInAir = MovementComponent->MovementMode == EMovementMode::MOVE_Falling;
		//const float Velocity = MovementComponent->GetVelocity();
	}

	const FRotator ControlRotation = OwningPawn->GetControlRotation();
	const FRotator Rotation = OwningPawn->GetActorRotation();
	const FVector Velocity  = OwningPawn->GetVelocity();
	Direction = Super::CalculateDirection(Velocity, Rotation);

	const FRotator ResultRotation = NormalizedDeltaRotator(ControlRotation, Rotation);
	Yaw = ResultRotation.Yaw;
	Pitch = ResultRotation.Pitch;

	SetCrouch();
	SetEquip();
	BlendWeight = (IsEquip) ? 0.8f : 0.f;
}

FRotator UCharacterAnimInstanceBase::NormalizedDeltaRotator(FRotator A, FRotator B) const
{
	FRotator Diff = A - B;
	Diff.Normalize();
	return Diff;
}

void UCharacterAnimInstanceBase::SetCrouch()
{
	if (this->Owner)
	{
		this->IsCrouch = this->Owner->HasCrouch();
	}
}

void UCharacterAnimInstanceBase::SetEquip()
{
	if (this->Owner)
	{
		this->IsEquip = this->Owner->HasEquipWeapon();
	}
}
