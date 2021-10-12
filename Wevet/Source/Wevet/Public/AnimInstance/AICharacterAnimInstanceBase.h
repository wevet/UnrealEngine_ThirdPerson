// Copyright 2018 wevet works All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CharacterAnimInstanceBase.h"
#include "AICharacterAnimInstanceBase.generated.h"

class AAICharacterBase;
/**
 * 
 */
UCLASS(transient, Blueprintable, hideCategories = AnimInstance, BlueprintType)
class WEVET_API UAICharacterAnimInstanceBase : public UCharacterAnimInstanceBase
{
	GENERATED_BODY()
	
public:
	UAICharacterAnimInstanceBase(const FObjectInitializer& ObjectInitializer);
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaTimeX) override;

protected:	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AI|Variable")
	float LookAtInterpSpeed;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AI|Variable")
	FName RifleGripSocketName;

protected:
	class AAICharacterBase* Character;
	virtual void CalculateAimOffset() override;

	virtual void SetWeaponFabrikIKTransform() override;
};
