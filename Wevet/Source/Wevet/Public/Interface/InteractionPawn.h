// Copyright 2018 wevet works All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "WevetTypes.h"
#include "InteractionPawn.generated.h"


/*
*	Require CharacterBase
*/
UINTERFACE(BlueprintType)
class WEVET_API UInteractionPawn : public UInterface
{
	GENERATED_UINTERFACE_BODY()
};

class WEVET_API IInteractionPawn
{
	GENERATED_IINTERFACE_BODY()

public :
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "InteractionPawn")
	void Pickup(const EItemType InItemType, AActor* Actor);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "InteractionPawn")
	const bool CanPickup();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "InteractionPawn")
	void Release();
};
