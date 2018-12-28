// Copyright 2018 wevet works All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AnimInstanceFullbodyIK.h"
#include "CharacterBase.h"
#include "CharacterAnimInstanceBase.generated.h"

/**
 * 
 */
UCLASS(transient, Blueprintable, hideCategories = AnimInstance, BlueprintType)
class WEVET_API UCharacterAnimInstanceBase : public UAnimInstanceFullbodyIK
{
	GENERATED_BODY()
	
public:
	UCharacterAnimInstanceBase(const FObjectInitializer& ObjectInitializer);
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaTimeX) override;

protected:
	APawn* OwningPawn;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Variable")
	ACharacterBase* Owner;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Variable")
	bool IsMoving;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Variable")
	bool IsFalling;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Variable")
	bool IsCrouch;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Variable")
	bool IsEquip;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Variable")
	float Speed;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Variable")
	float Direction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Variable")
	float Yaw;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Variable")
	float Pitch;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Variable")
	float BlendWeight;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Variable")
	float MaxBlendWeight;

	// FRotator Delta
	virtual FRotator NormalizedDeltaRotator(FRotator A, FRotator B) const;

	virtual void SetRotator();
	virtual void SetCrouch();
	virtual void SetEquip();
};
