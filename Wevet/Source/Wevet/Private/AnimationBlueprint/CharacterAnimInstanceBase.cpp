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

	this->OwningPawn = TryGetPawnOwner();
}

void UCharacterAnimInstanceBase::NativeUpdateAnimation(float DeltaTimeX)
{
	Super::NativeUpdateAnimation(DeltaTimeX);

	if (this->OwningPawn == nullptr) 
	{
		return;
	}

	this->Owner = Cast<ACharacterBase>(this->OwningPawn);
	this->IsMoving = (this->OwningPawn->GetVelocity().SizeSquared() > 25);
	this->Speed = this->OwningPawn->GetVelocity().Size();

	UPawnMovementComponent* MovementComponent = this->OwningPawn->GetMovementComponent();
	
	if (MovementComponent)
	{
		this->IsFalling = MovementComponent->IsFalling();
		//const bool isInAir = MovementComponent->MovementMode == EMovementMode::MOVE_Falling;
		//const float Velocity = MovementComponent->GetVelocity();
	}

	const FRotator ControlRotation = this->OwningPawn->GetControlRotation();
	const FRotator Rotation = this->OwningPawn->GetActorRotation();
	const FVector Velocity  = this->OwningPawn->GetVelocity();
	this->Direction = Super::CalculateDirection(Velocity, Rotation);

	const FRotator ResultRotation = NormalizedDeltaRotator(ControlRotation, Rotation);
	this->Yaw = ResultRotation.Yaw;
	this->Pitch = ResultRotation.Pitch;

	SetCrouch();
	SetEquip();
	this->BlendWeight = (this->IsEquip) ? 0.8f : 0.f;
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
