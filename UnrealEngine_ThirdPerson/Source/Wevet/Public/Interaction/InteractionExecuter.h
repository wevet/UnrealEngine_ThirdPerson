// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "InteractionExecuter.generated.h"

/**
 * 
 */
UINTERFACE(BlueprintType)
class WEVET_API UInteractionExecuter : public UInterface
{
	GENERATED_UINTERFACE_BODY()
};

class WEVET_API IInteractionExecuter
{
	GENERATED_IINTERFACE_BODY()

public :

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "IInteractionExecuter")
	void OnReleaseItemExecuter();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "IInteractionExecuter")
	void OnPickupItemExecuter(AActor* Actor);
};
