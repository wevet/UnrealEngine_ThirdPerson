// Copyright 2018 wevet works All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "FullbodyAnimInstance.h"
#include "Character/CharacterBase.h"
#include "Interface/GrabInstigator.h"
#include "LocomotionSystemSpeed.h"
#include "CharacterAnimInstanceBase.generated.h"

/**
* inheritance FullbodyAnimInstance
*/
UCLASS(transient, Blueprintable, hideCategories = AnimInstance, BlueprintType)
class WEVET_API UCharacterAnimInstanceBase : public UFullbodyAnimInstance, public IGrabInstigator, public ILocomotionSystemSpeed
{
	GENERATED_BODY()
	
public:
	UCharacterAnimInstanceBase(const FObjectInitializer& ObjectInitializer);
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaTimeX) override;

protected:
	APawn* OwningPawn;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Variable")
	ACharacterBase* Owner;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Variable")
	bool bHasMoving;

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

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Variable")
	float MovementSpeed;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Variable")
	float CalcDirection;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Variable")
	float Yaw;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Variable")
	float Pitch;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Variable")
	float BlendWeight;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Variable")
	float CombatBlendWeight;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Variable")
	bool bWasMoving;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Variable")
	bool bWasAiming;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Variable")
	EWeaponItemType WeaponItemType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Variable")
	class UCharacterMovementComponent* CharacterMovementComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Variable")
	class UCapsuleComponent* CapsuleComponent;

	virtual FRotator NormalizedDeltaRotator(FRotator A, FRotator B) const;

	virtual void SetMovementSpeed();
	virtual void SetRotator();
	virtual void SetCrouch();
	virtual void SetEquip();
	virtual void SetHanging();
	virtual void SetClimbingLedge();
	virtual void SetClimbingMove();
	virtual void SetWeaponItemType();

#pragma region Climbsystem
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Climbsystem")
	bool IsHanging;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Climbsystem")
	bool IsClimbingLedge;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Climbsystem")
	bool IsClimbMoveLeft;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Climbsystem")
	bool IsClimbMoveRight;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Climbsystem")
	bool bCanClimbMoveLeft;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Climbsystem")
	bool bCanClimbMoveRight;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Climbsystem")
	bool bClimbJumping;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Climbsystem")
	float ClimbBlendWeight;
#pragma endregion

protected:
	UFUNCTION(BlueprintCallable, Category = CharacterAnimInstance)
	bool IsLocallyControlled() const;

#pragma region IGrabInstigator
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "GrabInstigator")
	void CanGrab(bool InCanGrab);
	virtual void CanGrab_Implementation(bool InCanGrab) override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "GrabInstigator")
	void ClimbLedge(bool InClimbLedge);
	virtual void ClimbLedge_Implementation(bool InClimbLedge) override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "GrabInstigator")
	void ReportClimbEnd();
	virtual void ReportClimbEnd_Implementation() override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "GrabInstigator")
	void ClimbMove(float Value);
	virtual void ClimbMove_Implementation(float Value) override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "GrabInstigator")
	void ClimbJump();
	virtual void ClimbJump_Implementation() override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "GrabInstigator")
	void ReportClimbJumpEnd();
	virtual void ReportClimbJumpEnd_Implementation() override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "GrabInstigator")
	void TurnConerLeftUpdate();
	virtual void TurnConerLeftUpdate_Implementation() override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "GrabInstigator")
	void TurnConerRightUpdate();
	virtual void TurnConerRightUpdate_Implementation() override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "GrabInstigator")
	void TurnConerResult();
	virtual void TurnConerResult_Implementation() override;
#pragma endregion

#pragma region ALSInterface
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AnimInstance|ALS_Speed")
	void SetWalkingSpeed(const float InWalkingSpeed);
	virtual void SetWalkingSpeed_Implementation(const float InWalkingSpeed) override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AnimInstance|ALS_Speed")
	void SetRunningSpeed(const float InRunningSpeed);
	virtual void SetRunningSpeed_Implementation(const float InRunningSpeed) override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AnimInstance|ALS_Speed")
	void SetSprintingSpeed(const float InSprintingSpeed);
	virtual void SetSprintingSpeed_Implementation(const float InSprintingSpeed) override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AnimInstance|ALS_Speed")
	void SetCrouchingSpeed(const float InCrouchingSpeed);
	virtual void SetCrouchingSpeed_Implementation(const float InCrouchingSpeed) override;
#pragma endregion

#pragma region ALS
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ALS")
	float WalkingSpeed;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ALS")
	float RunningSpeed;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ALS")
	float SprintingSpeed;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ALS")
	float CrouchingSpeed;
#pragma endregion
};
