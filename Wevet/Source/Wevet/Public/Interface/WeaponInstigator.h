// Copyright 2018 wevet works All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "WeaponInstigator.generated.h"


/*
*	Weapon Interface
*/
UINTERFACE(BlueprintType)
class WEVET_API UWeaponInstigator : public UInterface
{
	GENERATED_UINTERFACE_BODY()
};

class WEVET_API IWeaponInstigator
{
	GENERATED_IINTERFACE_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "WeaponInstigator")
	void DoFirePressed();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "WeaponInstigator")
	void DoFireRelease();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "WeaponInstigator")
	void DoMeleeAttack(const bool InEnableMeleeAttack);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "WeaponInstigator")
	void DoReload();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "WeaponInstigator")
	void DoReplenishment(const int32 InAddAmmo);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "WeaponInstigator")
	bool CanMeleeStrike() const;
};
