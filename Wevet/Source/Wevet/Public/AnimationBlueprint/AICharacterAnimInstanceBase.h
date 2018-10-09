// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AnimationBlueprint/CharacterAnimInstanceBase.h"
#include "AICharacterAnimInstanceBase.generated.h"

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
};
