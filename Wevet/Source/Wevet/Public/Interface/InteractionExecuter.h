// Copyright 2018 wevet works All Rights Reserved.

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
