#pragma once
#include "Engine/EngineTypes.h"
#include "WevetTypes.h"
#include "WeaponItemInfo.generated.h"


USTRUCT(BlueprintType)
struct WEVET_API FWeaponItemInfo
{
	GENERATED_USTRUCT_BODY();

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variable")
	EWeaponItemType WeaponItemType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variable")
	int32 ClipType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variable")
	int32 CurrentAmmo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variable")
	int32 MaxAmmo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variable")
	float Damage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variable")
	float TraceDistance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variable")
	class UTexture2D* Texture;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variable")
	FName EquipSocketName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variable")
	FName UnEquipSocketName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variable")
	float DamageRange;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variable")
	FString DisplayName;

	FWeaponItemInfo()
	{
		WeaponItemType = EWeaponItemType::None;
		EquipSocketName = FName(TEXT("Weapon_Socket"));
		Damage = 20.f;
		TraceDistance = 15000.f;
		DamageRange = 0.f;
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
	}

};

