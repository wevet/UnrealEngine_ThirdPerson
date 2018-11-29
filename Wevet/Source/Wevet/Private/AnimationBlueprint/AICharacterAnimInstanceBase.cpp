// Copyright 2018 wevet works All Rights Reserved.

#include "AICharacterAnimInstanceBase.h"
#include "GameFramework/Pawn.h"
#include "AICharacterBase.h"


UAICharacterAnimInstanceBase::UAICharacterAnimInstanceBase(const FObjectInitializer& ObjectInitializer) 
	: Super(ObjectInitializer)
{
	this->TargetSocketName = FName(TEXT("Spine_03"));
}

void UAICharacterAnimInstanceBase::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
}


void UAICharacterAnimInstanceBase::NativeUpdateAnimation(float DeltaTimeX)
{
	this->AICharacterOwner = Cast<AAICharacterBase>(this->OwningPawn);

	if (this->IsEquip)
	{
		SetPitch();
	}
	Super::NativeUpdateAnimation(DeltaTimeX);
}

void UAICharacterAnimInstanceBase::SetPitch()
{
	if (this->AICharacterOwner == nullptr)
	{
		return;
	}
	// @TODO AI LookAt 
}


