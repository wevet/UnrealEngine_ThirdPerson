// Copyright 2018 wevet works All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Character/CharacterBase.h"
#include "Curves/CurveFloat.h"
#include "LocomotionSystemTypes.h"
#include "LocomotionSystemStructs.h"
#include "LocomotionSystemPawn.h"
#include "LocomotionSystemAction.h"
#include "MockCharacter.generated.h"

class AMockPlayerController;
class UPlayerAnimInstance;

UCLASS(Blueprintable, BlueprintType)
class WEVET_API AMockCharacter : public ACharacterBase, public ILocomotionSystemPawn
{
	GENERATED_BODY()

public:
	AMockCharacter(const FObjectInitializer& ObjectInitializer);
	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

public:
	FORCEINLINE class USpringArmComponent* GetCameraBoomComponent() const { return CameraBoomComponent; }
	FORCEINLINE class UCameraComponent* GetTPSCameraComponent() const { return TPSCameraComponent; }
	FORCEINLINE class UCameraComponent* GetFPSCameraComponent() const { return FPSCameraComponent; }

public:
	// DamageInstigator
	virtual void Die_Implementation() override;
	virtual void Equipment_Implementation() override;
	virtual void UnEquipment_Implementation() override;

	// InteractionPawn
	virtual void Pickup_Implementation(const EItemType InItemType, AActor* Actor) override;
	virtual const bool CanPickup_Implementation() override;
	virtual void Release_Implementation() override;

	// Climb
	virtual void ClimbLedge_Implementation(bool InClimbLedge) override;
	virtual void ClimbJump_Implementation() override;
	virtual void TurnConerResult_Implementation() override;

	virtual FVector BulletTraceRelativeLocation() const override
	{
		return GetTPSCameraComponent()->GetComponentLocation();
	}

	virtual FVector BulletTraceForwardLocation() const override
	{
		return GetTPSCameraComponent()->GetForwardVector();
	}

	virtual void EquipmentActionMontage() override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoomComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* TPSCameraComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FPSCameraComponent;

protected:
	UPROPERTY()
	class AMockPlayerController* PlayerController;

	UPROPERTY()
	class UPlayerAnimInstance* PlayerAnimInstance;

	UPROPERTY()
	TArray<class UMeshComponent*> MeshArray;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Player|Variable")
	int32 RecoverHealthValue;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Player|Variable")
	float RecoverTimer;
	float RecoverInterval;

protected:
	virtual void OnCrouch() override;
	virtual void PickupObjects() override;
	virtual void ReleaseObjects() override;
	virtual void Jump() override;
	virtual void StopJumping() override;
	virtual void FirePressed() override;
	virtual void FireReleassed() override;
	virtual void Reload() override;

	void UpdateWeapon();
	void TurnAtRate(float Rate);
	void LookUpAtRate(float Rate);
	void MoveForward(float Value);
	void MoveRight(float Value);
	void ToggleEquip();
	void TickableRecover(const float InDeltaTime);

	UFUNCTION(BlueprintCallable, Category = "Player|Function")
	void SetOwnerNoSeeMesh(const bool NewOwnerNoSee);

private:
	int32 WeaponCurrentIndex;

#pragma region ALSInterface
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Player|ALS_Action")
	void SetALSCharacterRotation(const FRotator AddAmount);
	virtual void SetALSCharacterRotation_Implementation(const FRotator AddAmount) override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Player|ALS_Action")
	void SetALSCameraShake(TSubclassOf<class UCameraShake> InShakeClass, const float InScale);
	virtual void SetALSCameraShake_Implementation(TSubclassOf<class UCameraShake> InShakeClass, const float InScale) override;
#pragma endregion


#pragma region ALS
protected:
	UFUNCTION(BlueprintCallable, Category = "Player|Ragdoll")
	void StartRagdollAction();

	UFUNCTION(BlueprintCallable, Category = "Player|Ragdoll")
	void RagdollToWakeUpAction();

	UFUNCTION(BlueprintCallable, Category = "Player|Ragdoll")
	void ReleaseWeaponInventory();

	UFUNCTION(BlueprintCallable, Category = "Player|Ragdoll")
	void DoWhileRagdoll();

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Player|ALS")
	ELSGait ALSGait;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Player|ALS")
	ELSStance ALSStance;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Player|ALS")
	ELSRotationMode ALSRotationMode;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Player|ALS")
	ELSMovementMode ALSMovementMode;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Player|ALS")
	ELSMovementMode ALSPrevMovementMode;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Player|ALS")
	ELSViewMode ALSViewMode;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Player|ALS")
	bool bWasMoving;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Player|ALS")
	bool bAiming;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Player|ALS")
	bool bWasMovementInput;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Player|ALS")
	bool bRightShoulder;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Player|ALS")
	bool bRagdollOnGround;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Player|ALS")
	FName RagdollPoseSnapshot;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Player|ALS")
	FVector RagdollVelocity;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Player|ALS")
	float RotationRateMultiplier;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Player|ALS")
	float RotationOffset;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Player|ALS")
	FRotator JumpRotation;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Player|ALS")
	class UCurveFloat* CameraLerpCurve;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Player|ALS")
	ELSCardinalDirection CardinalDirection;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Player|ALS")
	struct FCameraSettings CurrentCameraSettings;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Player|ALS")
	struct FCameraSettingsTarget CameraTargets;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player|ALS")
	float AimYawDelta;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player|ALS")
	float AimYawRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player|ALS")
	float VelocityDifference;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player|ALS")
	float RotationDifference;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player|ALS")
	float Direction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player|ALS")
	FRotator LastVelocityRotation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player|ALS")
	FRotator LastMovementInputRotation;

public:
	UFUNCTION(BlueprintCallable, Category = "Player|ALS")
	FORCEINLINE ELSGait GetALSGait() const
	{
		return ALSGait;
	}

	UFUNCTION(BlueprintCallable, Category = "Player|ALS")
	FORCEINLINE ELSStance GetALSStance() const
	{
		return ALSStance;
	}

	UFUNCTION(BlueprintCallable, Category = "Player|ALS")
	FORCEINLINE ELSRotationMode GetALSRotationMode() const
	{
		return ALSRotationMode;
	}

	UFUNCTION(BlueprintCallable, Category = "Player|ALS")
	FORCEINLINE ELSMovementMode GetALSMovementMode() const
	{
		return ALSMovementMode;
	}

	UFUNCTION(BlueprintCallable, Category = "Player|ALS")
	FORCEINLINE ELSViewMode GetALSViewMode() const
	{
		return ALSViewMode;
	}

	UFUNCTION(BlueprintCallable, Category = "Player|ALS")
	FORCEINLINE bool HasMoving() const
	{
		return bWasMoving; 
	}

	UFUNCTION(BlueprintCallable, Category = "Player|ALS")
	FORCEINLINE bool HasAiming() const 
	{
		return bAiming; 
	}

	UFUNCTION(BlueprintCallable, Category = "Player|ALS")
	FORCEINLINE bool HasMovementInput() const 
	{
		return bWasMovementInput; 
	}

	UFUNCTION(BlueprintCallable, Category = "Player|ALS")
	FVector ChooseVelocity() const
	{
		if (ALSMovementMode == ELSMovementMode::Ragdoll)
		{
			return GetMesh()->GetPhysicsLinearVelocity(PelvisBoneName);
		}
		else if (ALSMovementMode == ELSMovementMode::Swimming)
		{
		}
		return Super::GetVelocity();
	}

	UFUNCTION(BlueprintCallable, Category = "Player|ALS")
	FORCEINLINE float ChooseMaxAcceleration() const
	{
		return (ALSGait == ELSGait::Walking) ? WalkingAcceleration : RunningAcceleration;
	}

	UFUNCTION(BlueprintCallable, Category = "Player|ALS")
	FORCEINLINE float ChooseBrakingDeceleration() const
	{
		return (ALSGait == ELSGait::Walking) ? WalkingDeceleration : RunningDeceleration;
	}

	UFUNCTION(BlueprintCallable, Category = "Player|ALS")
	FORCEINLINE float ChooseGroundFriction() const
	{
		return (ALSGait == ELSGait::Walking) ? WalkingGroundFriction : RunningGroundFriction;
	}

	UFUNCTION(BlueprintCallable, Category = "Player|ALS")
	float ChooseMaxWalkSpeed() const;

	UFUNCTION(BlueprintCallable, Category = "Player|ALS")
	bool CanSprint() const;

	UFUNCTION(BlueprintCallable, Category = "Player|ALS")
	const float CalculateRotationRate(const float SlowSpeed, const float SlowSpeedRate, const float FastSpeed, const float FastSpeedRate);

	UPROPERTY(Replicated, BlueprintReadWrite)
	FVector MovementInput;

	UPROPERTY(Replicated, BlueprintReadWrite)
	FVector RagdollLocation;

	UPROPERTY(Replicated, BlueprintReadWrite)
	FRotator CharacterRotation;

	UPROPERTY(Replicated, BlueprintReadWrite)
	FRotator LookingRotation;

	UPROPERTY(Replicated, BlueprintReadWrite)
	FRotator TargetRotation;

protected:
	UFUNCTION(BlueprintCallable, Category = "Player|ALS")
	uint8 DoifDifferentByte(const uint8 A, const uint8 B) const
	{
		return DoifDifferent<uint8>(A, B);
	}

	UFUNCTION(BlueprintCallable, Category = "Player|ALS")
	bool DoifDifferentBool(const bool A, const bool B) const
	{
		return DoifDifferent<bool>(A, B);
	}

	UFUNCTION(BlueprintCallable, Category = "Player|ALS")
	float DoIfDifferentFloat(const float A, const float B) const
	{
		return DoifDifferent<float>(A, B);
	}

	template<typename T>
	FORCEINLINE T DoifDifferent(const T A, const T B) const
	{
		if (A != B)
		{
			return A;
		}
		return B;
	}

public:
	UFUNCTION(BlueprintCallable, Category = "Player|ALS")
	void CustomAcceleration();

	UFUNCTION(BlueprintCallable, Category = "Player|ALS")
	void AddCharacterRotation(const FRotator AddAmount);

	UFUNCTION(BlueprintCallable, Category = "Player|ALS")
	void LimitRotation(const float AimYawLimit, const float InterpSpeed);

	UFUNCTION(BlueprintCallable, Category = "Player|ALS")
	void ApplyCharacterRotation(const FRotator InTargetRotation, const bool bInterpRotation, const float InterpSpeed);
#pragma endregion

};

