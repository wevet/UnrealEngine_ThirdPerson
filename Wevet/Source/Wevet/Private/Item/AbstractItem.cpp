// Copyright 2018 wevet works All Rights Reserved.


#include "Item/AbstractItem.h"
#include "Character/CharacterBase.h"
#include "Engine.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "WevetExtension.h"


AAbstractItem::AAbstractItem(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;

	CollisionComponent = ObjectInitializer.CreateDefaultSubobject<USphereComponent>(this, TEXT("CollisionComponent"));
	CollisionComponent->SetSphereRadius(90.0f);
	RootComponent = CollisionComponent;

	StaticMeshComponent = ObjectInitializer.CreateDefaultSubobject<UStaticMeshComponent>(this, TEXT("StaticMeshComponent"));
	StaticMeshComponent->SetupAttachment(CollisionComponent);
	StaticMeshComponent->bRenderCustomDepth = false;
	StaticMeshComponent->CustomDepthStencilValue = 0;
	StaticMeshComponent->bUseAttachParentBound = 1;

	WidgetComponent = ObjectInitializer.CreateDefaultSubobject<UWidgetComponent>(this, TEXT("WidgetComponent"));
	WidgetComponent->SetDrawSize(FVector2D(250.f, 70.f));
	WidgetComponent->SetWorldLocation(FVector(0.f, 0.f, 60.f));
	WidgetComponent->SetupAttachment(CollisionComponent);
	WidgetComponent->bUseAttachParentBound = 1;
	WidgetComponent->SetVisibility(false);
}


void AAbstractItem::BeginPlay()
{
	Super::BeginPlay();
	Super::SetActorTickEnabled(false);
	AddDelegate();
}


void AAbstractItem::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Target = nullptr;
	Super::EndPlay(EndPlayReason);
}


#pragma region Interface
void AAbstractItem::Release_Implementation(APawn* NewCharacter)
{
	if (PickupSound)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), PickupSound, GetActorLocation());
	}

	if (NewCharacter)
	{
		IInteractionPawn::Execute_OverlapActor(NewCharacter, nullptr);
	}

	Target = nullptr;
	RemoveDelegate();

	Super::MarkRenderStateDirty(StaticMeshComponent, false, ECustomDepthType::None);
	Super::SetActorTickEnabled(false);
	WidgetComponent->SetVisibility(false);

	if (IsValidLowLevel())
	{
		Super::Destroy();
		Super::ConditionalBeginDestroy();
	}

}


void AAbstractItem::SpawnToWorld_Implementation()
{
	if (StaticMeshComponent)
	{
		StaticMeshComponent->SetCollisionEnabled(ECollisionEnabled::Type::QueryAndPhysics);
		StaticMeshComponent->SetCollisionProfileName(TEXT("Ragdoll"));
		StaticMeshComponent->SetEnableGravity(true);
		StaticMeshComponent->SetSimulatePhysics(true);
		StaticMeshComponent->WakeAllRigidBodies();
	}
}


EItemType AAbstractItem::GetItemType_Implementation() const
{
	return WeaponAmmoInfo.BaseItemType;
}
#pragma endregion


#pragma region Override
void AAbstractItem::BeginOverlapRecieve(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::BeginOverlapRecieve(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
}


void AAbstractItem::EndOverlapRecieve(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	Super::EndOverlapRecieve(OverlappedComp, OtherActor, OtherComp, OtherBodyIndex);
}


void AAbstractItem::AddDelegate()
{
	if (CollisionComponent)
	{
		CollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &AAbstractItem::BeginOverlapRecieve);
		CollisionComponent->OnComponentEndOverlap.AddDynamic(this, &AAbstractItem::EndOverlapRecieve);
	}
}


void AAbstractItem::RemoveDelegate()
{
	if (CollisionComponent)
	{
		CollisionComponent->OnComponentBeginOverlap.RemoveDynamic(this, &AAbstractItem::BeginOverlapRecieve);
		CollisionComponent->OnComponentEndOverlap.RemoveDynamic(this, &AAbstractItem::EndOverlapRecieve);
	}
}


void AAbstractItem::OverlapActor(AActor* OtherActor)
{
	Target = OtherActor;
	const bool bWasHitResult = Target ? true : false;
	const ECustomDepthType DepthType = Target ? ECustomDepthType::Item : ECustomDepthType::None;
	WidgetComponent->SetVisibility(bWasHitResult);
	Super::SetActorTickEnabled(bWasHitResult);
	Super::MarkRenderStateDirty(StaticMeshComponent, bWasHitResult, DepthType);
}
#pragma endregion

