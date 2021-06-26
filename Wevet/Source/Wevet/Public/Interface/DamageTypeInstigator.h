// Copyright 2018 wevet works All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "WevetTypes.h"
#include "DamageTypeInstigator.generated.h"


UINTERFACE(BlueprintType)
class WEVET_API UDamageTypeInstigator : public UInterface
{
	GENERATED_UINTERFACE_BODY()
};

class WEVET_API IDamageTypeInstigator
{
	GENERATED_IINTERFACE_BODY()

public :
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "DamageTypeInstigator")
	EGiveDamageType GetGiveDamageType() const;

};
