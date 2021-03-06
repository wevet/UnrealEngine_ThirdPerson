// Copyright 2018 wevet works All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CharacterInventoryComponent.generated.h"

class AAbstractWeapon;
class AAbstractItem;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class WEVET_API UCharacterInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UCharacterInventoryComponent(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	TArray<class AAbstractWeapon*> WeaponArray;
	TArray<class AAbstractItem*> ItemArray;

public:
	void AddWeaponInventory(AAbstractWeapon* const NewWeaponBase);
	void RemoveWeaponInventory(AAbstractWeapon* const WeaponBase);
	void ClearWeaponInventory();
	const TArray<class AAbstractWeapon*>& GetWeaponInventory()
	{
		return WeaponArray;
	}

	void SetOwnerNoSeeMesh(const bool NewOwnerNoSee);
	AAbstractWeapon* GetUnEquipWeapon() const;
	AAbstractWeapon* GetNotEmptyWeapon() const;
	AAbstractWeapon* GetUnEquipWeaponByIndex(const int32 InIndex);
	AAbstractWeapon* FindByIndexWeapon(const int32 InIndex);

public:
	void AddItemInventory(AAbstractItem* const NewItemBase);
	void RemoveItemInventory(AAbstractItem* const ItemBase);
	void ClearItemInventory();
	const TArray<class AAbstractItem*>& GetItemInventory()
	{
		return ItemArray;
	}

public:
	void ReleaseAllWeaponInventory();
	void ReleaseAllItemInventory();

	bool EmptyWeaponInventory() const;
	bool EmptyItemInventory() const;

};
