// Copyright 2018 wevet works All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AnimInstanceFullbodyIK.h"
#include "CharacterBase.h"
#include "GrabExecuter.h"
#include "CharacterAnimInstanceBase.generated.h"

/**
 * 
 */
UCLASS(transient, Blueprintable, hideCategories = AnimInstance, BlueprintType)
class WEVET_API UCharacterAnimInstanceBase : public UAnimInstanceFullbodyIK, public IGrabExecuter
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
	bool IsFallout;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Variable")
	float FalloutInterval;
	float FalloutTickTime;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Variable")
	bool IsCrouch;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Variable")
	bool IsEquip;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Animation|Climbsystem")
	bool IsHanging;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Animation|Climbsystem")
	bool IsClimbingLedge;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Animation|Climbsystem")
	bool IsClimbMoveLeft;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Animation|Climbsystem")
	bool IsClimbMoveRight;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Animation|Climbsystem")
	bool bCanClimbMoveLeft;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Animation|Climbsystem")
	bool bCanClimbMoveRight;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Animation|Climbsystem")
	bool bClimbJumping;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Animation|Climbsystem")
	float ClimbBlendWeight;

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
	float CombatBlendWeight;

	// FRotator Delta
	virtual FRotator NormalizedDeltaRotator(FRotator A, FRotator B) const;

	virtual void SetRotator();
	virtual void SetCrouch();
	virtual void SetEquip();
	virtual void SetHanging();
	virtual void SetClimbingLedge();
	virtual void SetClimbingMove();


public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "UCharacterAnimInstanceBase|IGrabExecuter")
	void CanGrab(bool InCanGrab);
	virtual void CanGrab_Implementation(bool InCanGrab) override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "UCharacterAnimInstanceBase|IGrabExecuter")
	void ClimbLedge(bool InClimbLedge);
	virtual void ClimbLedge_Implementation(bool InClimbLedge) override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "UCharacterAnimInstanceBase|IGrabExecuter")
	void ReportClimbEnd();
	virtual void ReportClimbEnd_Implementation() override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "UCharacterAnimInstanceBase|IGrabExecuter")
	void ClimbMove(float Value);
	virtual void ClimbMove_Implementation(float Value) override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "UCharacterAnimInstanceBase|IGrabExecuter")
	void ClimbJump();
	virtual void ClimbJump_Implementation() override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "UCharacterAnimInstanceBase|IGrabExecuter")
	void ReportClimbJumpEnd();
	virtual void ReportClimbJumpEnd_Implementation() override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "UCharacterAnimInstanceBase|IGrabExecuter")
	void TurnConerLeftUpdate();
	virtual void TurnConerLeftUpdate_Implementation() override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "UCharacterAnimInstanceBase|IGrabExecuter")
	void TurnConerRightUpdate();
	virtual void TurnConerRightUpdate_Implementation() override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "UCharacterAnimInstanceBase|IGrabExecuter")
	void TurnConerResult();
	virtual void TurnConerResult_Implementation() override;
};
