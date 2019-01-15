// Copyright 2018 wevet works All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "STypes.h"
#include "WeaponControllerExecuter.generated.h"

class AWeaponBase;
class UTexture2D;

/*
* require interface
* AWeaponBase Class
*/
UINTERFACE(BlueprintType)
class WEVET_API UWeaponControllerExecuter : public UInterface
{
	GENERATED_UINTERFACE_BODY()
};

class WEVET_API IWeaponControllerExecuter
{
	GENERATED_IINTERFACE_BODY()

public:

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "IWeaponControllerExecuter")
	void OnFirePress();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "IWeaponControllerExecuter")
	void OnFireRelease();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "IWeaponControllerExecuter")
	void OnReloading();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "IWeaponControllerExecuter")
	void OffVisible();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "IWeaponControllerExecuter")
	void OnVisible();
};
