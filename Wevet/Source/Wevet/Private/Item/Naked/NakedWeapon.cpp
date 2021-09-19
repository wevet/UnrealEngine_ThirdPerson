// Copyright 2018 wevet works All Rights Reserved.


#include "Item/Naked/NakedWeapon.h"
#include "Character/CharacterBase.h"
#include "Engine.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Wevet.h"


ANakedWeapon::ANakedWeapon(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
	bRenderCutomDepthEnable = false;

	WeaponItemInfo.WeaponItemType = EWeaponItemType::Naked;
	WeaponItemInfo.MeleeDistance = 500.f;
	WeaponItemInfo.TraceDistance = 500.f;
	WeaponItemInfo.EquipSocketName = NAME_None;
	WeaponItemInfo.UnEquipSocketName = NAME_None;
	WeaponItemInfo.DisplayName = FString(TEXT("Bare Hands"));
	WeaponItemInfo.Priority = 0;

	MuzzleSocketName = NAME_None;
	GripSocketName = NAME_None;

	// Hidden Components
	SkeletalMeshComponent->SetVisibility(false);
	SkeletalMeshComponent->SetComponentTickEnabled(false);
	SkeletalMeshComponent->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);

	CollisionComponent->SetVisibility(false);
	CollisionComponent->SetComponentTickEnabled(false);
	CollisionComponent->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);
}


void ANakedWeapon::BeginPlay()
{
	Super::BeginPlay();
	Super::SetActorTickEnabled(false);
	NakedWeaponTriggerMap.Reset();
}


void ANakedWeapon::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	NakedWeaponTriggerMap.Reset();
}


#pragma region Public
void ANakedWeapon::Initialize(APawn* const NewCharacterOwner)
{
	Super::Initialize(NewCharacterOwner);

	if (GetPawnOwner())
	{
		DoDeployTemplate();
	}
}
#pragma endregion


#pragma region Interface
void ANakedWeapon::DoFirePressed_Implementation()
{
	Super::DoFirePressed_Implementation();
	if (WeaponActionDelegate.IsBound())
	{
		WeaponActionDelegate.Broadcast(bCanFire);
	}
}


void ANakedWeapon::DoFireRelease_Implementation()
{
	Super::DoFireRelease_Implementation();
	if (WeaponActionDelegate.IsBound())
	{
		WeaponActionDelegate.Broadcast(bCanFire);
	}
}


bool ANakedWeapon::CanStrike_Implementation() const
{
	//unsafe pointer
	if (!CharacterPtr.IsValid())
	{
		return false;
	}

	if (ICombatInstigator* Interface = Cast<ICombatInstigator>(GetPawnOwner()))
	{
		if (Interface == nullptr)
		{
			return false;
		}

		if (ICombatInstigator::Execute_IsDeath(Interface->_getUObject()))
		{
			return false;
		}
	}

	if (!WasEquip())
	{
		UE_LOG(LogWevetClient, Log, TEXT("No Equip => %s"), *FString(__FUNCTION__));
		return false;
	}
	return true;
}
#pragma endregion


void ANakedWeapon::DoDeployTemplate()
{

}




