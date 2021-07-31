// Copyright 2018 wevet works All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "WevetTypes.h"
#include "InteractionItem.generated.h"

class APawn;

/*
*	Require ItemBase | AbstractWeapon classes
*/
UINTERFACE(BlueprintType)
class WEVET_API UInteractionItem : public UInterface
{
	GENERATED_UINTERFACE_BODY()
};

class WEVET_API IInteractionItem
{
	GENERATED_IINTERFACE_BODY()

public :
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "InteractionItem")
	void Take(APawn* NewCharacter);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "InteractionItem")
	void Release(APawn* NewCharacter);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "InteractionItem")
	EItemType GetItemType() const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "InteractionItem")
	void SpawnToWorld();
};
