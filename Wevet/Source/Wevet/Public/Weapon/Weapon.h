// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Weapon.generated.h"

UINTERFACE(BlueprintType)
class WEVET_API UWeapon : public UInterface
{
	GENERATED_UINTERFACE_BODY()
};


class WEVET_API IWeapon
{
	GENERATED_IINTERFACE_BODY()

public:

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interface")
	void OnFirePress();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interface")
	void OnFireRelease();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interface")
	void OnReloading();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interface")
	void OffVisible();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interface")
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
};


