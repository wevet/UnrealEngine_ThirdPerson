// Copyright 2018 wevet works All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "DamageInstigator.generated.h"


class UCharacterModel;

/*
* require interface
* ACharacterBase Class
*/
UINTERFACE(BlueprintType)
class WEVET_API UDamageInstigator : public UInterface
{
	GENERATED_UINTERFACE_BODY()
};

class WEVET_API IDamageInstigator
{
	GENERATED_IINTERFACE_BODY()

public:
	// Damage Sender Owner
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "DamageInstigator")
	void InfrictionDamage(AActor* InfrictionActor, const bool bInfrictionDie);

	// Handle Death
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "DamageInstigator")
	bool IsDeath();

	// KillEvent
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "DamageInstigator")
	void Die();

	// WeaponEquipment
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "DamageInstigator")
	void Equipment();

	// WeaponUnEquipment
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "DamageInstigator")
	void UnEquipment();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "DamageInstigator")
	UCharacterModel* GetPropertyModel() const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "DamageInstigator")
	void MakeDamage(UCharacterModel* DamageModel, const int InWeaponDamage, float& OutDamage);

	// etc HeadShot Damage onkill
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "DamageInstigator")
	bool CanKillDealDamage(const FName BoneName) const;
};
