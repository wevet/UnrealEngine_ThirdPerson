// Copyright 2018 wevet works All Rights Reserved.

#include "Item/Trigger/WeaponTriggerBase.h"
#include "Character/CharacterBase.h"
#include "WevetExtension.h"
#include "Wevet.h"


AWeaponTriggerBase::AWeaponTriggerBase(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
	bWasHitResult = false;
	bWasOverlapResult = false;
}


void AWeaponTriggerBase::BeginPlay()
{
	Super::BeginPlay();
	//PrimitiveComponent = Cast<UPrimitiveComponent>(GetComponentByClass(UPrimitiveComponent::StaticClass()));
}


void AWeaponTriggerBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	IgnoreActors.Reset(0);
	Super::EndPlay(EndPlayReason);
}


void AWeaponTriggerBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}


void AWeaponTriggerBase::Initialize(const TArray<class AActor*>& InOwners)
{
	IgnoreActors = InOwners;
}


void AWeaponTriggerBase::BeginOverlapRecieve(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!HasOtherOwner(OtherActor))
	{
		return;
	}

	if (OtherActor->ActorHasTag(DAMAGE_TAG))
	{
		if (WeaponTriggerHitDelegate.IsBound())
		{
			WeaponTriggerHitDelegate.Broadcast(OtherActor, SweepResult);
		}
	}
}


void AWeaponTriggerBase::HitReceive(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	
}


bool AWeaponTriggerBase::HasOtherOwner(AActor* OtherActor) const
{
	if (!OtherActor || OtherActor == this)
	{
		return false;
	}

	if (IgnoreActors.Contains(OtherActor))
	{
		UE_LOG(LogWevetClient, Warning, TEXT("Ignores Actor : %s, funcName : %s"), *OtherActor->GetName(), *FString(__FUNCTION__));
		return false;
	}
	return true;
}

