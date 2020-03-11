// Copyright 2018 wevet works All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "WevetTypes.h"
#include "InteractionInstigator.generated.h"

class ACharacterBase;

/*
*	Require ItemBase | AbstractWeapon classes
*/
UINTERFACE(BlueprintType)
class WEVET_API UInteractionInstigator : public UInterface
{
	GENERATED_UINTERFACE_BODY()
};

class WEVET_API IInteractionInstigator
{
	GENERATED_IINTERFACE_BODY()

public :
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "InteractionInstigator")
	void Take(ACharacterBase* NewCharacter);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "InteractionInstigator")
	void Release(ACharacterBase* NewCharacter);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "InteractionInstigator")
	EItemType GetItemType() const;
};
