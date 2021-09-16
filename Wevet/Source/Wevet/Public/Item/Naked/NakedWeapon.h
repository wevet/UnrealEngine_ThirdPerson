// Copyright 2018 wevet works All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Item/AbstractWeapon.h"
#include "NakedWeapon.generated.h"

UCLASS()
class WEVET_API ANakedWeapon : public AAbstractWeapon
{
	GENERATED_BODY()
	
public:
	ANakedWeapon(const FObjectInitializer& ObjectInitializer);
};
