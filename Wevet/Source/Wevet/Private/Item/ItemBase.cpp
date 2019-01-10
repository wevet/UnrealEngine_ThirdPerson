// Copyright © 2018 wevet works All Rights Reserved.

#include "ItemBase.h"
#include "CharacterBase.h"
#include "BulletBase.h"
#include "CharacterPickupComponent.h"
#include "CharacterModel.h"
#include "Engine.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

AItemBase::AItemBase(const FObjectInitializer& ObjectInitializer) 
	: Super(ObjectInitializer),
	WidgetComponent(nullptr),
	SphereComponent(nullptr),
	StaticMeshComponent(nullptr)
{
	PrimaryActorTick.bCanEverTick = false;

	SceneComponent = ObjectInitializer.CreateDefaultSubobject<USceneComponent>(this, TEXT("SceneComponent"));
	RootComponent = SceneComponent;

	StaticMeshComponent = ObjectInitializer.CreateDefaultSubobject<UStaticMeshComponent>(this, TEXT("StaticMeshComponent"));
	StaticMeshComponent->SetupAttachment(RootComponent);
	SphereComponent = ObjectInitializer.CreateDefaultSubobject<USphereComponent>(this, TEXT("SphereComponent"));
	SphereComponent->SetSphereRadius(90.0f);
	SphereComponent->SetupAttachment(StaticMeshComponent);
	WidgetComponent = ObjectInitializer.CreateDefaultSubobject<UWidgetComponent>(this, TEXT("WidgetComponent"));
	WidgetComponent->SetDrawSize(FVector2D(250.f, 70.f));
	WidgetComponent->SetWorldLocation(FVector(0.f, 0.f, 60.f));
	WidgetComponent->SetupAttachment(StaticMeshComponent);
}

void AItemBase::BeginPlay()
{
	Super::BeginPlay();
	
	if (ensure(WidgetComponent && WidgetComponent->IsValidLowLevel()))
	{
		WidgetComponent->SetVisibility(false);
	}

	if (ensure(SphereComponent && SphereComponent->IsValidLowLevel()))
	{
		SphereComponent->OnComponentBeginOverlap.AddDynamic(this, &AItemBase::BeginOverlapRecieve);
		SphereComponent->OnComponentEndOverlap.AddDynamic(this, &AItemBase::EndOverlapRecieve);
	}
}

void AItemBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AItemBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void AItemBase::BeginOverlapRecieve(
	UPrimitiveComponent* OverlappedComponent, 
	AActor* OtherActor, 
	UPrimitiveComponent* OtherComp, 
	int32 OtherBodyIndex, 
	bool bFromSweep, 
	const FHitResult & SweepResult)
{
	WidgetComponent->SetVisibility(true);
	StaticMeshComponent->SetRenderCustomDepth(true);

	if (ACharacterBase* Character = Cast<ACharacterBase>(OtherActor))
	{
		Character->GetPickupComponent()->SetPickupActor(this);

		//const auto Start = GetActorLocation();
		//const auto Target = Character->GetActorLocation();
		//FRotator Rotation = UKismetMathLibrary::FindLookAtRotation(Start, Target);
		//Rotation.Roll  = 0.f;
		//Rotation.Pitch = 0.f;
		//WidgetComponent->SetRelativeRotation(Rotation);
	}
}

void AItemBase::EndOverlapRecieve(
	UPrimitiveComponent* OverlappedComp, 
	AActor* OtherActor, 
	UPrimitiveComponent* OtherComp, 
	int32 OtherBodyIndex)
{
	WidgetComponent->SetVisibility(false);
	StaticMeshComponent->SetRenderCustomDepth(false);

	if (ACharacterBase* Character = Cast<ACharacterBase>(OtherActor))
	{
		Character->GetPickupComponent()->SetPickupActor(nullptr);
	}
}

void AItemBase::TakeResult()
{
	if (PickupSoundAsset)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), PickupSoundAsset, GetActorLocation());
	}
	if (IsValidLowLevel())
	{
		Super::Destroy();
		Super::ConditionalBeginDestroy();
	}
}
