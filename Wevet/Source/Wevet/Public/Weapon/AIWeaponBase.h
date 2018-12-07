// Copyright © 2018 wevet works All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Weapon/WeaponBase.h"
#include "AIWeaponBase.generated.h"

/**
 * 
 */
class AAICharacterBase;

UCLASS()
class WEVET_API AAIWeaponBase : public AWeaponBase
{
	GENERATED_BODY()
	
public:
	AAIWeaponBase(const FObjectInitializer& ObjectInitializer);

	//UFUNCTION(BlueprintCallable, Category = "AAIWeaponBase|Override")
	virtual void OnFirePressedInternal() override;

	virtual void SetCharacterOwner(ACharacterBase* InCharacterOwner) override;

protected:
	AAICharacterBase* Owner;
};
