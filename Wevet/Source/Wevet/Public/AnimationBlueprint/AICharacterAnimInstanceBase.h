// Copyright 2018 wevet works All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AnimationBlueprint/CharacterAnimInstanceBase.h"
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
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variable")
	FName HandSocketName;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variable")
	FName TargetSocketName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Variable")
	AAICharacterBase* AICharacterOwner;

	virtual void SetPitch();
};
