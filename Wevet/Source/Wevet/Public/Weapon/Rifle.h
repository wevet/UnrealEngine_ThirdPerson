// Copyright 2018 wevet works All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Weapon/AbstractWeapon.h"
#include "Rifle.generated.h"


UCLASS()
class WEVET_API ARifle : public AAbstractWeapon
{
	GENERATED_BODY()

public:
	ARifle(const FObjectInitializer& ObjectInitializer);

};
