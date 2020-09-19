#include "Weapon/BulletBase.h"
#include "Character/CharacterBase.h"
#include "WevetExtension.h"
#include "Wevet.h"

ABulletBase::ABulletBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer),
	bWasHitResult(false),
	bWasOverlapResult(false)
{
	PrimaryActorTick.bCanEverTick = true;
	LifeInterval = 2.0f;

	Tags.Add(FName(TEXT("Water")));

	{
		static ConstructorHelpers::FObjectFinder<UParticleSystem> FindAsset(TEXT("/Game/VFX/Cascade/Gameplay/Water/P_splash_bullet_impact"));
		ImpactWaterEmitterTemplate = FindAsset.Object;
	}

	{
		static ConstructorHelpers::FObjectFinder<UParticleSystem> FindAsset(TEXT("/Game/VFX/Cascade/Gameplay/Blood/P_body_bullet_impact"));
		ImpactBloodEmitterTemplate = FindAsset.Object;
	}
}

void ABulletBase::BeginPlay()
{
	Super::BeginPlay();
	Super::SetLifeSpan(LifeInterval);

	PrimitiveComponent = Cast<UPrimitiveComponent>(GetComponentByClass(UPrimitiveComponent::StaticClass()));
	ParticleComponent = Cast<UParticleSystemComponent>(GetComponentByClass(UParticleSystemComponent::StaticClass()));

	if (PrimitiveComponent)
	{
		PrimitiveComponent->SetCollisionProfileName(FName(TEXT("WorldDynamic")));
		PrimitiveComponent->SetGenerateOverlapEvents(true);
		PrimitiveComponent->SetNotifyRigidBodyCollision(true);
		PrimitiveComponent->OnComponentBeginOverlap.AddDynamic(this, &ABulletBase::BeginOverlapRecieve);
		PrimitiveComponent->OnComponentHit.AddDynamic(this, &ABulletBase::HitReceive);
		PrimitiveComponent->ComponentTags.Add(FName(TEXT("WaterLocal")));
	}
}

void ABulletBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (PrimitiveComponent) 
	{
		PrimitiveComponent->OnComponentBeginOverlap.RemoveDynamic(this, &ABulletBase::BeginOverlapRecieve);
		PrimitiveComponent->OnComponentHit.RemoveDynamic(this, &ABulletBase::HitReceive);
	}
	IgnoreActors.Reset(0);
	Super::EndPlay(EndPlayReason);
}

void ABulletBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ABulletBase::SetOwners(const TArray<class AActor*>& InOwners)
{
	IgnoreActors = InOwners;
}

void ABulletBase::BeginOverlapRecieve(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor || OtherActor == this)
		return;

	if (IgnoreActors.Contains(OtherActor))
	{
		UE_LOG(LogWevetClient, Warning, TEXT("Ignores Actor : %s"), *OtherActor->GetName());
		return;
	}
	
	if (OtherActor->IsA(ACharacterBase::StaticClass()))
	{
		//UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactBloodEmitterTemplate, GetActorTransform(), true);
	}

	if (OtherActor->ActorHasTag(WATER_BODY_TAG))
	{
		if (!bWasOverlapResult)
		{
			bWasOverlapResult = true;
			UParticleSystemComponent* PS = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactWaterEmitterTemplate, GetActorTransform(), true);
			PS->ComponentTags.Add(WATER_TAG);
			PS->GetOwner()->Tags.Add(WATER_TAG);
		}
	} 
	else
	{
		if (!OtherActor->IsA(ACharacterBase::StaticClass()))
		{
			UE_LOG(LogWevetClient, Log, TEXT("OtherActor => %s, funcName => %s"), *OtherActor->GetName(), *FString(__FUNCTION__));
		}
	}
	Super::Destroy();
}

void ABulletBase::HitReceive(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (!OtherActor || OtherActor == this)
		return;

	Super::Destroy();
}
