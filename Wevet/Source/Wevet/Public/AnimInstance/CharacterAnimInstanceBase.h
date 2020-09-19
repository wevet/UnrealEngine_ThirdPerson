// Copyright 2018 wevet works All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "FullbodyAnimInstance.h"
#include "Character/CharacterBase.h"
#include "LocomotionSystemSpeed.h"
#include "LocomotionSystemTypes.h"
#include "LocomotionSystemPropertyGetter.h"
#include "CharacterAnimInstanceBase.generated.h"

/**
* inheritance FullbodyAnimInstance
*/
UCLASS(transient, Blueprintable, hideCategories = AnimInstance, BlueprintType)
class WEVET_API UCharacterAnimInstanceBase : public UFullbodyAnimInstance, public ILocomotionSystemSpeed
{
	GENERATED_BODY()

public:
	UCharacterAnimInstanceBase(const FObjectInitializer& ObjectInitializer);
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaTimeX) override;

protected:
	APawn* OwningPawn;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Variable")
	class ACharacterBase* Owner;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Variable")
	class UCharacterMovementComponent* CharacterMovementComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Variable")
	class UCapsuleComponent* CapsuleComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Variable")
	bool IsFalling;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Variable")
	bool IsFallout;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Variable")
	bool IsCrouch;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Variable")
	bool IsEquip;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Variable")
	bool bWasMoving;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Variable")
	bool bWasAiming;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Variable")
	bool bWasStanning;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Variable")
	float FalloutInterval;
	float FalloutTickTime;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Variable")
	float MovementSpeed;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Variable")
	float CalcDirection;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Variable")
	float Yaw;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Variable")
	float Pitch;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Variable")
	EWeaponItemType WeaponItemType;

	virtual FRotator NormalizedDeltaRotator(FRotator A, FRotator B) const;

	virtual void SetStanning();
	virtual void SetMoving();
	virtual void SetMovementSpeed();
	virtual void SetRotator();
	virtual void SetCrouch();
	virtual void SetEquip();
	virtual void SetWeaponItemType();

public:
	FORCEINLINE FRotator GetAimRotation() const { return FRotator(Pitch, Yaw, 0.0f); }
	void RagdollToWakeUpAction(const bool InFaceDown);

protected:
	UFUNCTION(BlueprintCallable, Category = CharacterAnimInstance)
	bool IsLocallyControlled() const;

public:
#pragma region ALSInterface
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

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AnimInstance|ALS_Speed")
	void SetSwimmingSpeed(const float InSwimmingSpeed);
	virtual void SetSwimmingSpeed_Implementation(const float InSwimmingSpeed) override;
#pragma endregion

protected:
#pragma region ALS
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ALS")
	class UAnimMontage* GetUpFromBack;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ALS")
	class UAnimMontage* GetUpFromFront;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ALS")
	float WalkingSpeed;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ALS")
	float RunningSpeed;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ALS")
	float SprintingSpeed;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ALS")
	float CrouchingSpeed;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ALS")
	float SwimmingSpeed;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ALS")
	ELSMovementMode ALSMovementMode;
#pragma endregion


};
