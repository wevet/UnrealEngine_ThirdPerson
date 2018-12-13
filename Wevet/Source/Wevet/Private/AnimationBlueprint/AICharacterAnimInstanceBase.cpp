// Copyright 2018 wevet works All Rights Reserved.

#include "AICharacterAnimInstanceBase.h"
#include "GameFramework/Pawn.h"
#include "AICharacterBase.h"


UAICharacterAnimInstanceBase::UAICharacterAnimInstanceBase(const FObjectInitializer& ObjectInitializer) 
	: Super(ObjectInitializer),
	TargetSocketName(TEXT("Spine_03"))
{
	//
}

void UAICharacterAnimInstanceBase::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
}

void UAICharacterAnimInstanceBase::NativeUpdateAnimation(float DeltaTimeX)
{
	AICharacterOwner = Cast<AAICharacterBase>(OwningPawn);

	if (IsEquip)
	{
		SetPitch();
	}
	Super::NativeUpdateAnimation(DeltaTimeX);
}

void UAICharacterAnimInstanceBase::SetPitch()
{
	if (AICharacterOwner == nullptr)
	{
		return;
	}
	// @TODO AI LookAt 
}


