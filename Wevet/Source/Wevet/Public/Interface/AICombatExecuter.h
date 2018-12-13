// Copyright 2018 wevet works All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "AICombatExecuter.generated.h"

/**
 *
 */
UINTERFACE(BlueprintType)
class WEVET_API UAICombatExecuter : public UInterface
{
	GENERATED_UINTERFACE_BODY()
};

class WEVET_API IAICombatExecuter
{
	GENERATED_IINTERFACE_BODY()

public:

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "UAICombatExecuter")
	void Patrolling();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "UAICombatExecuter")
	void CheckEnemySighting();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "UAICombatExecuter")
	void Hunting();
};
