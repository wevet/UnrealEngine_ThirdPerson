// Copyright 2018 wevet works All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "WevetTypes.h"
#include "AttackInstigator.generated.h"



UINTERFACE(BlueprintType)
class WEVET_API UAttackInstigator : public UInterface
{
	GENERATED_UINTERFACE_BODY()
};

class WEVET_API IAttackInstigator
{
	GENERATED_IINTERFACE_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AttackInstigator")
	void DoFirePressed();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AttackInstigator")
	void DoFireReleassed();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AttackInstigator")
	void DoMeleeAttack();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AttackInstigator")
	void DoReload();
};
