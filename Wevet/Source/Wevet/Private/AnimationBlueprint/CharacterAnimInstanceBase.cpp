// Fill out your copyright notice in the Description page of Project Settings.

#include "CharacterAnimInstanceBase.h"
#include "Kismet/KismetSystemLibrary.h"
#include "GameFramework/Pawn.h"

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

	//UPawnMovementComponent* comp = this->OwningPawn->GetMovementComponent();
	//const bool falling = this->OwningPawn->GetMovementComponent()->IsFalling();

	const FRotator ControlRotation = this->OwningPawn->GetControlRotation();
	const FRotator Rotation = this->OwningPawn->GetActorRotation();
	const FVector Velocity  = this->OwningPawn->GetVelocity();
	this->Direction = Super::CalculateDirection(Velocity, Rotation);

	const FRotator ResultRotation = NormalizedDeltaRotator(ControlRotation, Rotation);
	this->Yaw = ResultRotation.Yaw;
	this->Pitch = ResultRotation.Pitch;
}

FRotator UCharacterAnimInstanceBase::NormalizedDeltaRotator(FRotator A, FRotator B) const
{
	FRotator Diff = A - B;
	Diff.Normalize();
	return Diff;
}
