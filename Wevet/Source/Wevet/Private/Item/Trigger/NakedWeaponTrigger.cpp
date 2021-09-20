// Copyright 2018 wevet works All Rights Reserved.


#include "Item/Trigger/NakedWeaponTrigger.h"
#include "Character/CharacterBase.h"
#include "WevetExtension.h"
#include "Wevet.h"


ANakedWeaponTrigger::ANakedWeaponTrigger(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
	NakedWeaponTriggerType = ENakedWeaponTriggerType::None;
	AttachBoneName = NAME_None;
	AddtionalDamage = 10.f;

	CollisionComponent = ObjectInitializer.CreateDefaultSubobject<UBoxComponent>(this, TEXT("CollisionComponent"));
	RootComponent = CollisionComponent;

}


void ANakedWeaponTrigger::BeginPlay()
{
	Super::BeginPlay();
	Super::SetActorTickEnabled(false);

	PrimitiveComponent = CollisionComponent;
}


void ANakedWeaponTrigger::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (PrimitiveComponent)
	{
		PrimitiveComponent->OnComponentBeginOverlap.RemoveDynamic(this, &ANakedWeaponTrigger::BeginOverlapRecieve);
	}
	Super::EndPlay(EndPlayReason);
}


void ANakedWeaponTrigger::BeginOverlapRecieve(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor || OtherActor == this)
	{
		return;
	}

	if (IgnoreActors.Contains(OtherActor))
	{
		UE_LOG(LogWevetClient, Warning, TEXT("Ignores Actor : %s"), *OtherActor->GetName());
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


void ANakedWeaponTrigger::NakedActionApply(const bool Enable)
{

}


