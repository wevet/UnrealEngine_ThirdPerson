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
	bShowMesh = false;

	StaticMeshComponent = ObjectInitializer.CreateDefaultSubobject<UStaticMeshComponent>(this, TEXT("StaticMeshComponent"));
	StaticMeshComponent->SetCollisionProfileName(FName(TEXT("WorldDynamic")));
	StaticMeshComponent->SetGenerateOverlapEvents(false);
	StaticMeshComponent->SetNotifyRigidBodyCollision(false);
	StaticMeshComponent->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);
	RootComponent = StaticMeshComponent;
}


void ANakedWeaponTrigger::BeginPlay()
{
	Super::BeginPlay();
	Super::SetActorTickEnabled(false);

	PrimitiveComponent = Cast<UStaticMeshComponent>(GetComponentByClass(UStaticMeshComponent::StaticClass()));

	if (PrimitiveComponent)
	{
		PrimitiveComponent->OnComponentBeginOverlap.AddDynamic(this, &ANakedWeaponTrigger::BeginOverlapRecieve);
		PrimitiveComponent->SetVisibility(bShowMesh);

		UE_LOG(LogWevetClient, Log, TEXT("PrimitiveComp Found => %s"), *FString(__FUNCTION__));
	}
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
	Super::BeginOverlapRecieve(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
}


void ANakedWeaponTrigger::NakedActionApply(const bool Enable)
{
	if (PrimitiveComponent)
	{
		PrimitiveComponent->SetGenerateOverlapEvents(Enable);
		PrimitiveComponent->SetNotifyRigidBodyCollision(Enable);
		const ECollisionEnabled::Type CollisionType = Enable ? ECollisionEnabled::Type::QueryOnly : ECollisionEnabled::Type::NoCollision;
		PrimitiveComponent->SetCollisionEnabled(CollisionType);
	}
}


