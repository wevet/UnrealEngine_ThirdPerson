// Copyright 2018 wevet works All Rights Reserved.

#include "AnimInstance/AICharacterAnimInstanceBase.h"
#include "GameFramework/Pawn.h"
#include "AI/AICharacterBase.h"
#include "Kismet/KismetMathLibrary.h"

UAICharacterAnimInstanceBase::UAICharacterAnimInstanceBase(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	LookAtInterpSpeed = 2.4f;
	Character = nullptr;
}


void UAICharacterAnimInstanceBase::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	Character = Cast<AAICharacterBase>(OwningPawn);
}


void UAICharacterAnimInstanceBase::NativeUpdateAnimation(float DeltaTimeX)
{
	Super::NativeUpdateAnimation(DeltaTimeX);
}


void UAICharacterAnimInstanceBase::CalculateAimOffset()
{
#if false

	if (!Character)
	{
		Super::CalculateAimOffset();
		return;
	}

	if (Character->GetTarget_Implementation())
	{
		const FVector Start = Character->GetActorLocation();
		const FVector Target = Character->GetTarget_Implementation()->GetActorLocation();
		const FRotator CurrentRotation = Character->GetActorRotation();
		const FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(Start, Target);	
		AimOffset.Y = FMath::Clamp(LookAtRotation.Pitch, -180.f, 180.f);
		AimOffset.X = FMath::Clamp(LookAtRotation.Yaw, -180.f, 180.f);
	}
	else
	{
		Super::CalculateAimOffset();
	}
#endif

}


void UAICharacterAnimInstanceBase::SetWeaponFabrikIKTransform()
{
	if (!Owner)
	{
		return;
	}

	auto Weapon = Owner->GetCurrentWeapon();
	if (!Weapon)
	{
		return;
	}

	switch (WeaponItemType)
	{
		case EWeaponItemType::Pistol:
		{
			Super::SetWeaponFabrikIKTransform();
		}
		break;
		case EWeaponItemType::Rifle:
		case EWeaponItemType::Sniper:
		{
			Super::SetHandTransform(Weapon->GetGripTransform(RifleGripSocketName), FTransform::Identity);
		}
		break;
	}
}

