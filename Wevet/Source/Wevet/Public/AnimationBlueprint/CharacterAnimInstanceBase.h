// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "CharacterBase.h"
#include "CharacterAnimInstanceBase.generated.h"

/**
 * 
 */
UCLASS(transient, Blueprintable, hideCategories = AnimInstance, BlueprintType)
class WEVET_API UCharacterAnimInstanceBase : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	UCharacterAnimInstanceBase(const FObjectInitializer& ObjectInitializer);

	virtual void NativeInitializeAnimation() override;

	virtual void NativeUpdateAnimation(float DeltaTimeX) override;

protected:
	APawn * OwningPawn;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Valiable")
	ACharacterBase* Owner;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Valiable")
	bool IsMoving;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Valiable")
	bool IsFalling;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Valiable")
	float Speed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Valiable")
	float Direction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Valiable")
	float Yaw;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Valiable")
	float Pitch;
	
	// FRotator Delta
	virtual FRotator NormalizedDeltaRotator(FRotator A, FRotator B) const;
};