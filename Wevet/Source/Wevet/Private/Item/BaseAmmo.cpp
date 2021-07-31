// Copyright 2018 wevet works All Rights Reserved.


#include "Item/BaseAmmo.h"
#include "Character/CharacterBase.h"
#include "Engine.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "WevetExtension.h"


ABaseAmmo::ABaseAmmo(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	bRenderCutomDepthEnable = true;
}


void ABaseAmmo::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (Target)
	{
		const FVector StartLocation = GetActorLocation();
		const FVector TargetLocation = Target->GetActorLocation();
		const FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(StartLocation, TargetLocation);
		WidgetComponent->SetWorldRotation(FRotator(0.0f, LookAtRotation.Yaw, 0.0f));
	}
}

