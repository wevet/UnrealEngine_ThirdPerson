// Copyright 2018 wevet works All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Character/CharacterBase.h"
#include "Components/PostProcessComponent.h"
#include "LocomotionSystemPawn.h"
#include "LocomotionSystemAction.h"
#include "MockCharacter.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FAliveDelegate);

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
	virtual void OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode = 0) override;
	virtual void OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;
	virtual void OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;

public:
	FORCEINLINE class USpringArmComponent* GetCameraBoomComponent() const { return CameraBoomComponent; }
	FORCEINLINE class UCameraComponent* GetTPSCameraComponent() const { return TPSCameraComponent; }
	FORCEINLINE class UCameraComponent* GetFPSCameraComponent() const { return FPSCameraComponent; }

public:
	UPROPERTY(BlueprintAssignable)
	FAliveDelegate AliveDelegate;

#pragma region Interface
public:
	// DamageInstigator
	virtual void Die_Implementation() override;
	virtual void Alive_Implementation() override;
	virtual void Equipment_Implementation() override;
	virtual void UnEquipment_Implementation() override;
	// InteractionPawn
	virtual void Pickup_Implementation(const EItemType InItemType, AActor* Actor) override;
	virtual const bool CanPickup_Implementation() override;
	virtual void Release_Implementation() override;
#pragma endregion

public:
	virtual FVector BulletTraceRelativeLocation() const override
	{
		return bAiming ? GetTPSCameraComponent()->GetComponentLocation() : Super::BulletTraceRelativeLocation();
	}

	virtual FVector BulletTraceForwardLocation() const override
	{
		const FVector ForwardLocation = bAiming ? GetTPSCameraComponent()->GetForwardVector() : GetActorForwardVector();
		const float TraceDistance = CurrentWeapon.IsValid() ? CurrentWeapon.Get()->GetTraceDistance() : ZERO_VALUE;
		return BulletTraceRelativeLocation() + (ForwardLocation * TraceDistance);
	}

	virtual void OverlapActor(AActor* InActor) override
	{
		if (OutlinePostProcessComponent)
		{
			OutlinePostProcessComponent->bEnabled = InActor ? 1 : 0;
			OutlinePostProcessComponent->bUnbound = InActor ? 1 : 0;
			OutlinePostProcessComponent->BlendWeight = InActor ? 1.0f : 0.0f;
			OutlinePostProcessComponent->SetVisibility(InActor != nullptr);
		}
		Super::OverlapActor(InActor);
	}

	virtual void EquipmentActionMontage() override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoomComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* TPSCameraComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FPSCameraComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components, meta = (AllowPrivateAccess = "true"))
	class UPostProcessComponent* OutlinePostProcessComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components, meta = (AllowPrivateAccess = "true"))
	class UPostProcessComponent* DeathPostProcessComponent;

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

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Player|Variable")
	FRotator FPSSocketRotation;

	float RecoverInterval;
	int32 WeaponCurrentIndex;

protected:
	void TickableRecover(const float InDeltaTime);

	UFUNCTION(BlueprintCallable, Category = "Player|Function")
	void SetOwnerNoSeeMesh(const bool NewOwnerNoSee);

	virtual void VisibleDeathPostProcess(const bool InEnabled);

public:
	int32 GetSelectWeaponIndex() const { return WeaponCurrentIndex; }

	bool WasFocus() const
	{
		if (!bAiming)
		{
			return false;
		}
		return HasEquipWeapon();
	}

#pragma region InputAction
public:
	virtual void Jump() override;
	virtual void StopJumping() override;
	virtual void Sprint() override;
	virtual void StopSprint() override;
	virtual void FirePressed() override;
	virtual void FireReleassed() override;
	virtual void MeleeAttack() override;
	virtual void Reload() override;
	virtual void OnCrouch() override;

protected:
	virtual void TurnAtRate(float Rate) override;
	virtual void LookUpAtRate(float Rate) override;
	virtual void MoveForward(float Value) override;
	virtual void MoveRight(float Value) override;
	virtual void ReleaseObjects() override;
	virtual void PickupObjects() override;

	void PlayerMovementInput(const bool bForwardAxis);
	void GroundMovementInput(const bool bForwardAxis);
	void RagdollMovementInput();
	void Aiming();
	void StopAiming();
	void UpdateWeapon();
	void ToggleEquip();
#pragma endregion

#pragma region ALSInterface
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Player|ALS_Action")
	void SetALSCharacterRotation(const FRotator AddAmount);
	virtual void SetALSCharacterRotation_Implementation(const FRotator AddAmount) override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Player|ALS_Action")
	void SetALSCameraShake(TSubclassOf<class UCameraShake> InShakeClass, const float InScale);
	virtual void SetALSCameraShake_Implementation(TSubclassOf<class UCameraShake> InShakeClass, const float InScale) override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Player|ALS_Action")
	void SetALSMovementMode(const ELSMovementMode NewALSMovementMode);
	virtual void SetALSMovementMode_Implementation(const ELSMovementMode NewALSMovementMode) override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Player|ALS_Action")
	void OnALSMovementModeChange();
	virtual void OnALSMovementModeChange_Implementation() override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Player|ALS_Action")
	void SetALSMovementAction(const ELSMovementAction NewALSMovementAction);
	virtual void SetALSMovementAction_Implementation(const ELSMovementAction NewALSMovementAction) override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Player|ALS_Action")
	void OnALSMovementActionChange();
	virtual void OnALSMovementActionChange_Implementation() override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Player|ALS_Action")
	void SetALSGait(const ELSGait NewALSGait);
	virtual void SetALSGait_Implementation(const ELSGait NewALSGait) override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Player|ALS_Action")
	void OnALSGaitChange();
	virtual void OnALSGaitChange_Implementation() override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Player|ALS_Action")
	void SetALSStance(const ELSStance NewALSStance);
	virtual void SetALSStance_Implementation(const ELSStance NewALSStance) override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Player|ALS_Action")
	void OnALSStanceChange();
	virtual void OnALSStanceChange_Implementation() override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Player|ALS_Action")
	void SetALSRotationMode(const ELSRotationMode NewALSRotationMode);
	virtual void SetALSRotationMode_Implementation(const ELSRotationMode NewALSRotationMode) override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Player|ALS_Action")
	void OnALSRotationModeChange();
	virtual void OnALSRotationModeChange_Implementation() override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Player|ALS_Action")
	void SetALSViewMode(const ELSViewMode NewALSViewMode);
	virtual void SetALSViewMode_Implementation(const ELSViewMode NewALSViewMode) override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Player|ALS_Action")
	void OnALSViewModeChange();
	virtual void OnALSViewModeChange_Implementation() override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Player|ALS_Action")
	void SetALSAiming(const bool NewALSAiming);
	virtual void SetALSAiming_Implementation(const bool NewALSAiming) override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Player|ALS_Action")
	void OnALSAimingChange();
	virtual void OnALSAimingChange_Implementation() override;
#pragma endregion

#pragma region ALSRagdoll
protected:
	virtual void StartRagdollAction() override;
	virtual void RagdollToWakeUpAction() override;

	UFUNCTION(BlueprintCallable, Category = "Player|ALS")
	void DoWhileRagdoll(FRotator& OutActorRotation, FVector& OutActorLocation);
	void CalculateActorTransformRagdoll(const FRotator InRagdollRotation, const FVector InRagdollLocation, FRotator& OutActorRotation, FVector& OutActorLocation);
#pragma endregion

#pragma region ALSUpdate
protected:
	void CalculateEssentialVariables();

	UFUNCTION(BlueprintCallable, Category = "Player|ALS")
	void ManageCharacterRotation();

	UFUNCTION(BlueprintCallable, Category = "Player|ALS")
	void DoCharacterFalling();

	UFUNCTION(BlueprintCallable, Category = "Player|ALS")
	void DoCharacterGrounded();

	UFUNCTION(BlueprintCallable, Category = "Player|ALS")
	void DoWhileGrounded();
#pragma endregion

#pragma region ALSProperty
protected:
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
	struct FCameraSettings CurrentCameraSettings;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Player|ALS")
	struct FCameraSettingsTarget CameraTargets;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Player|ALS")
	class UCurveFloat* DefaultCurve;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Player|ALS")
	class UCurveFloat* FastCurve;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Player|ALS")
	class UCurveFloat* SlowCurve;

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
#pragma endregion

#pragma region ALSAPI
public:
	FORCEINLINE bool HasMoving() const { return bWasMoving; }
	FORCEINLINE bool HasAiming() const { return bAiming; }

	virtual bool HasMovementInput_Implementation() const override
	{
		return bWasMovementInput;
	}

	FORCEINLINE bool HasDebugTrace() const { return bDebugTrace; }

	FVector ChooseVelocity() const
	{
		if (ALSMovementMode == ELSMovementMode::Ragdoll)
		{
			return GetMesh()->GetPhysicsLinearVelocity(PelvisBoneName);
		}
		return Super::GetVelocity();
	}

	FORCEINLINE float ChooseMaxAcceleration() const
	{
		return (ALSGait == ELSGait::Walking) ? WalkingAcceleration : RunningAcceleration;
	}

	FORCEINLINE float ChooseBrakingDeceleration() const
	{
		return (ALSGait == ELSGait::Walking) ? WalkingDeceleration : RunningDeceleration;
	}

	FORCEINLINE float ChooseGroundFriction() const
	{
		return (ALSGait == ELSGait::Walking) ? WalkingGroundFriction : RunningGroundFriction;
	}

	float ChooseMaxWalkSpeed() const;

	const float CalculateRotationRate(const float SlowSpeed, const float SlowSpeedRate, const float FastSpeed, const float FastSpeedRate);
	const FRotator LookingDirectionWithOffset(const float OffsetInterpSpeed, const float NEAngle, const float NWAngle, const float SEAngle, const float SWAngle, const float Buffer);

public:
	UPROPERTY(Replicated, BlueprintReadWrite)
	FVector MovementInput;

	UPROPERTY(Replicated, BlueprintReadWrite)
	FVector RagdollLocation;

	UPROPERTY(Replicated, BlueprintReadWrite)
	FRotator CharacterRotation;

	UPROPERTY(Replicated, BlueprintReadWrite)
	FRotator LookingRotation;

	//UPROPERTY(Replicated, BlueprintReadWrite)
	//FRotator TargetRotation;

protected:
	UFUNCTION(BlueprintCallable, Category = "Player|ALS")
	bool CanSprint() const;

	UFUNCTION(BlueprintCallable, Category = "Player|ALS")
	void AddCharacterRotation(const FRotator AddAmount);

	UFUNCTION(BlueprintCallable, Category = "Player|ALS")
	void UpdateCharacterMovementSettings();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Player|ALS")
	void BP_UpdateCameraAction(class UCurveFloat* LerpCurve);

protected:
	uint8 DoifDifferentByte(const uint8 A, const uint8 B) const { return DoifDifferent<uint8>(A, B); }
	bool DoifDifferentBool(const bool A, const bool B) const { return DoifDifferent<bool>(A, B); }

	UFUNCTION(BlueprintCallable, Category = "Player|ALS")
	float DoifDifferentFloat(const float A, const float B) const { return DoifDifferent<float>(A, B); }

	template<typename T>
	T DoifDifferent(const T A, const T B) const
	{
		if (A != B)
		{
			return A;
		}
		return B;
	}

protected:
	void ApplyCharacterRotation(const FRotator InTargetRotation, const bool bInterpRotation, const float InterpSpeed);
	void LimitRotation(const float AimYawLimit, const float InterpSpeed);
	bool CardinalDirectionAngles(const float Value, const float Min, const float Max, const float Buffer, const ELSCardinalDirection InCardinalDirection) const;
	void CustomAcceleration();
#pragma endregion

};

