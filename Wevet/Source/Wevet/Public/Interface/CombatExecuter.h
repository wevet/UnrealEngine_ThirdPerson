// Copyright 2018 wevet works All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "CombatExecuter.generated.h"

class USoundBase;

/*
* require interface
* ACharacterBase Class
*/
UINTERFACE(BlueprintType)
class WEVET_API UCombatExecuter : public UInterface
{
	GENERATED_UINTERFACE_BODY()
};

class WEVET_API ICombatExecuter
{
	GENERATED_IINTERFACE_BODY()

public:
	// TakeDamage
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ICombatExecuter")
	void OnTakeDamage(FName BoneName, float Damage, AActor* Actor);

	// Handle Death
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ICombatExecuter")
	bool IsDeath();

	// KillEvent
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ICombatExecuter")
	void Die();

	// WeaponEquipment
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ICombatExecuter")
	void Equipment();

	// WeaponUnEquipment
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ICombatExecuter")
	void UnEquipment();
};
