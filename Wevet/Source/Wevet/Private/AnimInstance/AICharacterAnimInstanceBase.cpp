// Copyright 2018 wevet works All Rights Reserved.

#include "AnimInstance/AICharacterAnimInstanceBase.h"
#include "GameFramework/Pawn.h"
#include "AI/AICharacterBase.h"
#include "Kismet/KismetMathLibrary.h"

UAICharacterAnimInstanceBase::UAICharacterAnimInstanceBase(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	LookAtInterpSpeed = 2.4f;
}

void UAICharacterAnimInstanceBase::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	Character = Cast<AAICharacterBase>(OwningPawn);
}

void UAICharacterAnimInstanceBase::NativeUpdateAnimation(float DeltaTimeX)
{
	Super::NativeUpdateAnimation(DeltaTimeX);

	if (Character)
	{
		ALSMovementMode = ILocomotionSystemPropertyGetter::Execute_GetALSMovementMode(Character);
	}
}

void UAICharacterAnimInstanceBase::SetRotator()
{
	if (!Character)
	{
		return;
	}

	if (Character->GetTarget_Implementation())
	{
		const FVector Start  = Character->GetActorLocation();
		const FVector Target = Character->GetTarget_Implementation()->GetActorLocation();
		const FRotator CurrentRotation = Character->GetActorRotation();
		const FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(Start, Target);
		Pitch = FMath::Clamp(LookAtRotation.Pitch, -90.f, 90.f);
		//Yaw = FMath::Clamp(FMath::Abs(LookAtRotation.Yaw), -180.f, 180.f);
		
		const FRotator ResultRotation = UKismetMathLibrary::RInterpTo(
			CurrentRotation, FRotator(0.0f, LookAtRotation.Yaw, 0.0f),
			GetWorld()->GetDeltaSeconds(), LookAtInterpSpeed);
		Character->SetActorRelativeRotation(FRotator(0.0f, ResultRotation.Yaw, 0.0f));
	}
	else
	{
		Super::SetRotator();
	}
}

void UAICharacterAnimInstanceBase::SetMovementSpeed()
{
	if (IAIPawnOwner::Execute_CanMeleeStrike(Character))
	{
	}
	Super::SetMovementSpeed();
}
