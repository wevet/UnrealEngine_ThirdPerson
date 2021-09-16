// Copyright 2018 wevet works All Rights Reserved.

#include "Item/Naked/NakedWeaponTrigger.h"
#include "Character/CharacterBase.h"
#include "WevetExtension.h"
#include "Wevet.h"


ANakedWeaponTrigger::ANakedWeaponTrigger(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
	NakedWeaponTriggerType = ENakedWeaponTriggerType::None;
	AttachBoneName = NAME_None;
	bWasHitResult = false;
	bWasOverlapResult = false;
}


void ANakedWeaponTrigger::BeginPlay()
{
	Super::BeginPlay();
	Super::SetActorTickEnabled(false);

	PrimitiveComponent = Cast<UPrimitiveComponent>(GetComponentByClass(UPrimitiveComponent::StaticClass()));

	if (PrimitiveComponent)
	{
		PrimitiveComponent->SetCollisionProfileName(FName(TEXT("WorldDynamic")));
		PrimitiveComponent->SetGenerateOverlapEvents(true);
		PrimitiveComponent->SetNotifyRigidBodyCollision(true);
		PrimitiveComponent->OnComponentBeginOverlap.AddDynamic(this, &ANakedWeaponTrigger::BeginOverlapRecieve);
		PrimitiveComponent->ComponentTags.Add(WATER_LOCAL_TAG);
	}
}


void ANakedWeaponTrigger::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (PrimitiveComponent)
	{
		PrimitiveComponent->OnComponentBeginOverlap.RemoveDynamic(this, &ANakedWeaponTrigger::BeginOverlapRecieve);
	}
	IgnoreActors.Reset(0);
	Super::EndPlay(EndPlayReason);
}


void ANakedWeaponTrigger::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
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

	}
}


void ANakedWeaponTrigger::SetOwners(const TArray<class AActor*>& InOwners)
{
	IgnoreActors = InOwners;
}


