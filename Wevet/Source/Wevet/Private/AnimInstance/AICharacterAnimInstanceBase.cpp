// Copyright 2018 wevet works All Rights Reserved.

#include "AnimInstance/AICharacterAnimInstanceBase.h"
#include "GameFramework/Pawn.h"
#include "AI/AICharacterBase.h"
#include "Kismet/KismetMathLibrary.h"

UAICharacterAnimInstanceBase::UAICharacterAnimInstanceBase(const FObjectInitializer& ObjectInitializer) 
	: Super(ObjectInitializer)
{
	//
}

void UAICharacterAnimInstanceBase::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
}

void UAICharacterAnimInstanceBase::NativeUpdateAnimation(float DeltaTimeX)
{
	if (AICharacterOwner == nullptr)
	{
		AICharacterOwner = Cast<AAICharacterBase>(OwningPawn);
	}
	Super::NativeUpdateAnimation(DeltaTimeX);
}

void UAICharacterAnimInstanceBase::SetRotator()
{
	Super::SetRotator();
	if (AICharacterOwner == nullptr || IsEquip == false)
	{
		return;
	}

	if (AICharacterOwner->GetTarget_Implementation() == nullptr)
	{
		return;
	}

	const FVector Start = AICharacterOwner->GetActorLocation();
	const FVector Target = AICharacterOwner->GetTarget_Implementation()->GetActorLocation();
	const FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(Start, Target);
	FRotator Rot = FRotator::ZeroRotator;
	Pitch = LookAtRotation.Pitch;
	//Yaw = LookAtRotation.Yaw;
	Rot.Yaw = LookAtRotation.Yaw;
	AICharacterOwner->SetActorRelativeRotation(Rot);
}
