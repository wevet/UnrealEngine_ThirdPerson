// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "CombatControllerExecuter.generated.h"

/**
 * 
 */
UINTERFACE(BlueprintType)
class WEVET_API UCombatControllerExecuter : public UInterface
{
	GENERATED_UINTERFACE_BODY()
};

class WEVET_API ICombatControllerExecuter
{
	GENERATED_IINTERFACE_BODY()

public:
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "UCombatControllerExecuter")
	void Patrolling();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "UCombatControllerExecuter")
	void CheckEnemySighting();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "UCombatControllerExecuter")
	void Hunting();
};
