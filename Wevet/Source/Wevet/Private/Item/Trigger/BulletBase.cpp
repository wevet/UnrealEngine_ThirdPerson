// Copyright 2018 wevet works All Rights Reserved.

#include "Item/Trigger/BulletBase.h"
#include "Character/CharacterBase.h"
#include "WevetExtension.h"
#include "Wevet.h"


ABulletBase::ABulletBase(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
	LifeInterval = 4.0f;

	Tags.Add(WATER_TAG);

	{
		static ConstructorHelpers::FObjectFinder<UParticleSystem> FindAsset(Wevet::ProjectFile::GetSplashBulletImpactPath());
		ImpactWaterEmitterTemplate = FindAsset.Object;
	}

	{
		static ConstructorHelpers::FObjectFinder<UParticleSystem> FindAsset(Wevet::ProjectFile::GetBulletImpactPath());
		ImpactBloodEmitterTemplate = FindAsset.Object;
	}
}


void ABulletBase::BeginPlay()
{
	Super::BeginPlay();
	Super::SetLifeSpan(LifeInterval);

	ParticleComponent = Wevet::ComponentExtension::GetComponentFirstOrDefault<UParticleSystemComponent>(this);
	if (ParticleComponent)
	{
		ParticleComponent->ComponentTags.Add(WATER_TAG);
	}

	if (PrimitiveComponent)
	{
		PrimitiveComponent->SetCollisionProfileName(FName(TEXT("WorldDynamic")));
		PrimitiveComponent->SetGenerateOverlapEvents(true);
		PrimitiveComponent->SetNotifyRigidBodyCollision(true);
		PrimitiveComponent->OnComponentBeginOverlap.AddDynamic(this, &ABulletBase::BeginOverlapRecieve);
		PrimitiveComponent->OnComponentHit.AddDynamic(this, &ABulletBase::HitReceive);
		PrimitiveComponent->ComponentTags.Add(WATER_LOCAL_TAG);
		//PrimitiveComponent->ComponentTags.Add(WATER_TAG);
	}
}


void ABulletBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (PrimitiveComponent) 
	{
		PrimitiveComponent->OnComponentBeginOverlap.RemoveDynamic(this, &ABulletBase::BeginOverlapRecieve);
		PrimitiveComponent->OnComponentHit.RemoveDynamic(this, &ABulletBase::HitReceive);
	}
	Super::EndPlay(EndPlayReason);
}


void ABulletBase::VisibleEmitter(const bool InVisible)
{
	if (ParticleComponent)
	{
		ParticleComponent->SetVisibility(InVisible);
	}
}


void ABulletBase::BeginOverlapRecieve(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::BeginOverlapRecieve(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

#if false
	if (OtherActor->ActorHasTag(WATER_BODY_TAG))
	{
		if (!bWasOverlapResult)
		{
			bWasOverlapResult = true;
			UParticleSystemComponent* PS = UGameplayStatics::SpawnEmitterAtLocation(
				GetWorld(), 
				ImpactWaterEmitterTemplate, 
				GetActorTransform(), 
				true);
			PS->ComponentTags.Add(WATER_TAG);
			PS->GetOwner()->Tags.Add(WATER_TAG);
		}
	} 
#endif

	if (HasOtherOwner(OtherActor))
	{
		Super::Destroy();
	}
}


void ABulletBase::HitReceive(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (!OtherActor || OtherActor == this)
	{
		return;
	}

	Super::Destroy();
}

