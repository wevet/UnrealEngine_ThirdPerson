// Fill out your copyright notice in the Description page of Project Settings.

#include "AICharacterAnimInstanceBase.h"
#include "Kismet/KismetSystemLibrary.h"
#include "GameFramework/Pawn.h"

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
	Super::NativeUpdateAnimation(DeltaTimeX);
}
