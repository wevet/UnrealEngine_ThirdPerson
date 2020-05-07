#include "Weapon/BulletBase.h"
#include "Character/CharacterBase.h"
#include "WevetExtension.h"

ABulletBase::ABulletBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer),
	bWasHit(false)
{
	PrimaryActorTick.bCanEverTick = true;
	LifeInterval = 4.0f;
}

void ABulletBase::BeginPlay()
{
	Super::BeginPlay();
	Super::SetLifeSpan(LifeInterval);

	PrimitiveComponent = Cast<UPrimitiveComponent>(GetComponentByClass(UPrimitiveComponent::StaticClass()));
	if (PrimitiveComponent)
	{
		PrimitiveComponent->SetGenerateOverlapEvents(true);
		PrimitiveComponent->SetNotifyRigidBodyCollision(true);
		PrimitiveComponent->OnComponentHit.AddDynamic(this, &ABulletBase::HitReceive);
	}
}

void ABulletBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (PrimitiveComponent)
	{
		PrimitiveComponent->OnComponentHit.RemoveDynamic(this, &ABulletBase::HitReceive);
	}
	IgnoreActors.Reset(0);
	Super::EndPlay(EndPlayReason);
}

void ABulletBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ABulletBase::SetOwners(const TArray<class AActor*> InOwners)
{
	IgnoreActors = InOwners;
}

void ABulletBase::HitReceive(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (bWasHit)
	{
		return;
	}
	bWasHit = true;
	if (HitTriggerDelegate.IsBound())
	{
		HitTriggerDelegate.Broadcast(Hit.GetActor(), Hit);
	}

	if (Wevet::ArrayExtension::NullOrEmpty(IgnoreActors) || IgnoreActors.Contains(Hit.GetActor()))
	{
		UE_LOG(LogWevetClient, Warning, TEXT("Ignores Actor : %s"), *Hit.GetActor()->GetName());
		return;
	}
	UE_LOG(LogWevetClient, Log, TEXT("Hit Actor : %s"), *Hit.GetActor()->GetName());
	Super::Destroy();
}
