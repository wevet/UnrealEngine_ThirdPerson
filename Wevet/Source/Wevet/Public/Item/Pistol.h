// Copyright 2018 wevet works All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbstractWeapon.h"
#include "Pistol.generated.h"


UCLASS()
class WEVET_API APistol : public AAbstractWeapon
{
	GENERATED_BODY()
	
public:
	APistol(const FObjectInitializer& ObjectInitializer);

};
