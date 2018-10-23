// Copyright 2018 wevet works All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "AICombatControllerExecuter.generated.h"

/**
 *
 */
UINTERFACE(BlueprintType)
class WEVET_API UAICombatControllerExecuter : public UInterface
{
	GENERATED_UINTERFACE_BODY()
};

class WEVET_API IAICombatControllerExecuter
{
	GENERATED_IINTERFACE_BODY()

public:

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "UAICombatControllerExecuter")
	void Patrolling();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "UAICombatControllerExecuter")
	void CheckEnemySighting();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "UAICombatControllerExecuter")
	void Hunting();
};
