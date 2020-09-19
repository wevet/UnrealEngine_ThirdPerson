#pragma once
#include "Engine/EngineTypes.h"
#include "WevetTypes.h"
#include "BaseItem.generated.h"


USTRUCT(BlueprintType)
struct WEVET_API FBaseItem
{
	GENERATED_USTRUCT_BODY();

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variable")
	FString DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variable")
	EItemType BaseItemType;

public:
	EItemType GetItemType() const
	{
		return BaseItemType; 
	}

	FORCEINLINE bool operator==(const FBaseItem& Other) const
	{
		return BaseItemType == Other.BaseItemType;
	}

	FBaseItem()
	{
		//
	}
};

USTRUCT(BlueprintType)
struct WEVET_API FWeaponItemInfo : public FBaseItem
{
	GENERATED_USTRUCT_BODY();

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variable")
	EWeaponItemType WeaponItemType;

	// Number of supplies
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variable")
	int32 ClipType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variable")
	int32 CurrentAmmo;

	// Total bullets count
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variable")
	int32 MaxAmmo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variable")
	float Damage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variable")
	float TraceDistance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variable")
	float MeleeDistance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variable")
	class UTexture2D* Texture;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variable")
	FName EquipSocketName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variable")
	FName UnEquipSocketName;

	FWeaponItemInfo() : FBaseItem()
	{
		BaseItemType = EItemType::Weapon;
		WeaponItemType = EWeaponItemType::None;
		EquipSocketName = FName(TEXT("Weapon_Socket"));
		Damage = 20.f;
		TraceDistance = 15000.f;
		MeleeDistance = 1000.f;
	}

public:

	void CopyTo(const FWeaponItemInfo& InWeaponItemInfo)
	{
		UnEquipSocketName = InWeaponItemInfo.UnEquipSocketName;
		EquipSocketName = InWeaponItemInfo.EquipSocketName;
		WeaponItemType = InWeaponItemInfo.WeaponItemType;
		CurrentAmmo = InWeaponItemInfo.CurrentAmmo;
		ClipType = InWeaponItemInfo.ClipType;
		MaxAmmo = InWeaponItemInfo.MaxAmmo;
		Damage = InWeaponItemInfo.Damage;
		Texture = InWeaponItemInfo.Texture;
		DisplayName = InWeaponItemInfo.DisplayName;
		MeleeDistance = InWeaponItemInfo.MeleeDistance;
	}

};

USTRUCT(BlueprintType)
struct WEVET_API FWeaponAmmoInfo : public FBaseItem
{
	GENERATED_USTRUCT_BODY();

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variable")
	EWeaponItemType WeaponItemType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variable")
	int32 Ammo;

	FWeaponAmmoInfo() : FBaseItem()
	{
		WeaponItemType = EWeaponItemType::None;
		BaseItemType = EItemType::Ammos;
		Ammo = 0;
	}
};

