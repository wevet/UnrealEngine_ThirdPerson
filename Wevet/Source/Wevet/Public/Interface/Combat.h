// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Combat.generated.h"

/**
*
*/
UINTERFACE(BlueprintType)
class WEVET_API UCombat : public UInterface
{
	GENERATED_UINTERFACE_BODY()
};


class WEVET_API ICombat
{
	GENERATED_IINTERFACE_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ICombatExecuter")
	void NotifyEquip();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ICombatExecuter")
	void OnTakeDamage(FName BoneName, float Damage, AActor* Actor);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ICombatExecuter")
	bool IsDeath();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ICombatExecuter")
	void Die();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ICombatExecuter")
	void Equipment();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ICombatExecuter")
	void UnEquipment();

};
