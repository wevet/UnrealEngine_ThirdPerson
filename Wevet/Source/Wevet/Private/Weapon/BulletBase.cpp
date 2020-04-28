#include "Weapon/BulletBase.h"


ABulletBase::ABulletBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer),
	bWasHit(false)
{
	PrimaryActorTick.bCanEverTick = true;
}

void ABulletBase::BeginPlay()
{
	Super::BeginPlay();
}

void ABulletBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ABulletBase::HitReceive(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
}
