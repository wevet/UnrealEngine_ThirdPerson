// Copyright 2018 wevet works All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GameFramework/Pawn.h"
#include "WevetTypes.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "CombatBlueprintFunctionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class WEVET_API UCombatBlueprintFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintPure, Category = Lib)
	static bool WeaponDropProbability();

	UFUNCTION(BlueprintPure, Category = Lib)
	static bool ItemDropProbability();

	UFUNCTION(BlueprintPure, Category = Lib)
	static bool CanDamagedActor(AActor* OtherActor, AActor* InSelf, APawn* InOwner);

	UFUNCTION(BlueprintPure, Category = Lib)
	static float CalcurateBaseDamage(const FHitResult& HitResult, AActor* InSelf, APawn* InOwner, const float InBaseDamage, const EWeaponItemType InWeaponItemType);

private:
	static bool Probability(const float InPercent);
};
