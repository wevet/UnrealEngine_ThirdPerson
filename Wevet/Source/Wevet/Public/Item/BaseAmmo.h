// Copyright 2018 wevet works All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Item/AbstractItem.h"
#include "BaseAmmo.generated.h"


UCLASS()
class WEVET_API ABaseAmmo : public AAbstractItem
{
	GENERATED_BODY()
	
public:
	ABaseAmmo(const FObjectInitializer& ObjectInitializer);
	virtual void Tick(float DeltaTime) override;

};
