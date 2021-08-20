// Copyright 2018 wevet works All Rights Reserved.

#include "Item/WorldItem.h"
#include "Components/PrimitiveComponent.h"
#include "Character/CharacterBase.h"
#include "WevetExtension.h"


AWorldItem::AWorldItem(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = false;
	bRenderCutomDepthEnable = false;

	static ConstructorHelpers::FObjectFinder<USoundBase> FindAsset(Wevet::ProjectFile::GetPickupSoundPath());
	PickupSound = FindAsset.Object;
}


void AWorldItem::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
}


void AWorldItem::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}


void AWorldItem::BeginDestroy()
{
	Super::BeginDestroy();
}


void AWorldItem::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	IgnoreActors.Reset(0);
	Super::EndPlay(EndPlayReason);
}


void AWorldItem::BeginPlay()
{
	IgnoreActors.Add(this);
	Super::BeginPlay();
}


void AWorldItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}


#pragma region Interface
void AWorldItem::Take_Implementation(APawn* NewCharacter)
{
}


void AWorldItem::Release_Implementation(APawn* NewCharacter)
{
}


void AWorldItem::SpawnToWorld_Implementation()
{
}


EItemType AWorldItem::GetItemType_Implementation() const
{
	return EItemType::None;
}
#pragma endregion


#pragma region HitEvent
void AWorldItem::BeginOverlapRecieve(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	IInteractionPawn* Interface = Cast<IInteractionPawn>(OtherActor);
	if (Interface == nullptr)
	{
		return;
	}

	if (IInteractionPawn::Execute_CanPickup(Interface->_getUObject()))
	{
		OverlapActor(OtherActor);
		IInteractionPawn::Execute_OverlapActor(Interface->_getUObject(), this);
	}
}


void AWorldItem::EndOverlapRecieve(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	IInteractionPawn* Interface = Cast<IInteractionPawn>(OtherActor);
	if (Interface == nullptr)
	{
		return;
	}

	if (IInteractionPawn::Execute_CanPickup(Interface->_getUObject()))
	{
		OverlapActor(nullptr);
		IInteractionPawn::Execute_OverlapActor(Interface->_getUObject(), nullptr);
	}
}


void AWorldItem::HitReceive(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
}
#pragma endregion


void AWorldItem::Initialize(APawn* const NewCharacterOwner)
{
	IgnoreActors.Add(NewCharacterOwner);
	Super::SetOwner(NewCharacterOwner);
}


void AWorldItem::MarkRenderStateDirty(UPrimitiveComponent* PrimitiveComponent, const bool InEnable, const ECustomDepthType InDepthType)
{
	if (!bRenderCutomDepthEnable)
	{
		return;
	}

	if (PrimitiveComponent)
	{
		PrimitiveComponent->SetRenderCustomDepth(InEnable);
		PrimitiveComponent->SetCustomDepthStencilValue(InEnable ? (int32)InDepthType : (int32)ECustomDepthType::None);
	}
}

