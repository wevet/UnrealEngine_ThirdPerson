// Copyright 2018 wevet works All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "WeaponControllerExecuter.generated.h"

/**
*
*/
UINTERFACE(BlueprintType)
class WEVET_API UWeaponControllerExecuter : public UInterface
{
	GENERATED_UINTERFACE_BODY()
};


class WEVET_API IWeaponControllerExecuter
{
	GENERATED_IINTERFACE_BODY()

public:

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "IWeaponControllerExecuter")
	void OnFirePress();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "IWeaponControllerExecuter")
	void OnFireRelease();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "IWeaponControllerExecuter")
	void OnReloading();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "IWeaponControllerExecuter")
	void OffVisible();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "IWeaponControllerExecuter")
	void OnVisible();
};


// enum
UENUM(BlueprintType)
enum class EWeaponItemType : uint8
{
	None   UMETA(DisplayName = "None"),
	Pistol UMETA(DisplayName = "Pistol"),
	Rifle  UMETA(DisplayName = "Rifle"),
	Sniper UMETA(DisplayName = "Sniper"),
};


// struct
USTRUCT(BlueprintType)
struct WEVET_API FWeaponItemInfo
{
	GENERATED_USTRUCT_BODY();

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variable")
	EWeaponItemType WeaponItemType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variable")
	TSubclassOf<class AWeaponBase> WeaponClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variable")
	int32 ClipType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variable")
	int32 CurrentAmmo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variable")
	int32 MaxAmmo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variable")
	float Damage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variable")
	UTexture2D* Texture;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variable")
	FName EquipSocketName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variable")
	FName UnEquipSocketName;

	FWeaponItemInfo();

public:
	void CopyTo(FWeaponItemInfo& InWeaponItemInfo)
	{
		WeaponItemType = InWeaponItemInfo.WeaponItemType;
		WeaponClass = InWeaponItemInfo.WeaponClass;
		ClipType = InWeaponItemInfo.ClipType;
		CurrentAmmo = InWeaponItemInfo.CurrentAmmo;
		MaxAmmo = InWeaponItemInfo.MaxAmmo;
		Damage = InWeaponItemInfo.Damage;
		Texture = InWeaponItemInfo.Texture;
		EquipSocketName = InWeaponItemInfo.EquipSocketName;
		UnEquipSocketName = InWeaponItemInfo.UnEquipSocketName;
	}
};


