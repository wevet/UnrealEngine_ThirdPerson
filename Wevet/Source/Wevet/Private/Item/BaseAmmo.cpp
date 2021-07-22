// Copyright 2018 wevet works All Rights Reserved.


#include "Item/BaseAmmo.h"
#include "Character/CharacterBase.h"
#include "Engine.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "WevetExtension.h"


ABaseAmmo::ABaseAmmo(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
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


void ABaseAmmo::BeginOverlapRecieve(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	IInteractionPawn* Interface = Cast<IInteractionPawn>(OtherActor);
	if (Interface == nullptr)
	{
		return;
	}

	if (IInteractionPawn::Execute_CanPickup(Interface->_getUObject()))
	{
		OverlapActor(OtherActor);
		UpdateCustomDepth(true);
		IInteractionPawn::Execute_OverlapActor(Interface->_getUObject(), this);
	}
}


void ABaseAmmo::EndOverlapRecieve(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	IInteractionPawn* Interface = Cast<IInteractionPawn>(OtherActor);
	if (Interface == nullptr)
	{
		return;
	}

	if (IInteractionPawn::Execute_CanPickup(Interface->_getUObject()))
	{
		OverlapActor(nullptr);
		UpdateCustomDepth(false);
		IInteractionPawn::Execute_OverlapActor(Interface->_getUObject(), nullptr);
	}
}


void ABaseAmmo::OverlapActor(AActor* OtherActor)
{
	Target = OtherActor;
	Super::SetActorTickEnabled(Target ? true : false);
	WidgetComponent->SetVisibility(Target ? true : false);
}

