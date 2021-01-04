// Copyright 2018 wevet works All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "FullbodyAnimInstance.h"

#include "LocomotionSystemPawn.h"
#include "LocomotionSystemStructs.h"
#include "LocomotionSystemNotify.h"

#include "Character/CharacterBase.h"
#include "Curves/CurveFloat.h"
#include "Curves/CurveVector.h"
#include "CharacterAnimInstanceBase.generated.h"

/**
* inheritance FullbodyAnimInstance
*/
UCLASS(transient, Blueprintable, hideCategories = AnimInstance, BlueprintType)
class WEVET_API UCharacterAnimInstanceBase : public UFullbodyAnimInstance, public ILocomotionSystemPawn, public ILocomotionSystemNotify
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

protected:
	UFUNCTION(BlueprintCallable, Category = CharacterAnimInstance)
	bool IsLocallyControlled() const;

public:
#pragma region ALSInterface
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AnimInstance|ALS_Pawn")
	ELSMovementMode GetALSMovementMode() const;
	virtual ELSMovementMode GetALSMovementMode_Implementation() const override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AnimInstance|ALS_Pawn")
	ELSMovementAction GetALSMovementAction() const;
	virtual ELSMovementAction GetALSMovementAction_Implementation() const override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AnimInstance|ALS_Pawn")
	ELSGait GetALSGait() const;
	virtual ELSGait GetALSGait_Implementation() const override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AnimInstance|ALS_Pawn")
	ELSStance GetALSStance() const;
	virtual ELSStance GetALSStance_Implementation() const override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AnimInstance|ALS_Pawn")
	ELSViewMode GetALSViewMode() const;
	virtual ELSViewMode GetALSViewMode_Implementation() const override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AnimInstance|ALS_Pawn")
	ELSRotationMode GetALSRotationMode() const;
	virtual ELSRotationMode GetALSRotationMode_Implementation() const override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AnimInstance|ALS_Pawn")
	bool HasMovementInput() const;
	virtual bool HasMovementInput_Implementation() const override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AnimInstance|ALS_Pawn")
	bool HasAiming() const;
	virtual bool HasAiming_Implementation() const override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AnimInstance|ALS_Pawn")
	FTransform GetPivotTarget() const;
	virtual FTransform GetPivotTarget_Implementation() const override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AnimInstance|ALS_Pawn")
	FVector GetCameraTarget() const;
	virtual FVector GetCameraTarget_Implementation() const override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AnimInstance|ALS_Pawn")
	void SetALSCharacterRotation(const FRotator AddAmount);
	virtual void SetALSCharacterRotation_Implementation(const FRotator AddAmount) override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AnimInstance|ALS_Pawn")
	void SetALSCameraShake(TSubclassOf<class UMatineeCameraShake> InShakeClass, const float InScale);
	virtual void SetALSCameraShake_Implementation(TSubclassOf<class UMatineeCameraShake> InShakeClass, const float InScale) override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AnimInstance|ALS_Pawn")
	void SetALSMovementMode(const ELSMovementMode NewALSMovementMode);
	virtual void SetALSMovementMode_Implementation(const ELSMovementMode NewALSMovementMode) override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AnimInstance|ALS_Pawn")
	void OnALSMovementModeChange();
	virtual void OnALSMovementModeChange_Implementation() override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AnimInstance|ALS_Pawn")
	void SetALSMovementAction(const ELSMovementAction NewALSMovementAction);
	virtual void SetALSMovementAction_Implementation(const ELSMovementAction NewALSMovementAction) override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AnimInstance|ALS_Pawn")
	void OnALSMovementActionChange();
	virtual void OnALSMovementActionChange_Implementation() override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AnimInstance|ALS_Pawn")
	void SetALSGait(const ELSGait NewALSGait);
	virtual void SetALSGait_Implementation(const ELSGait NewALSGait) override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AnimInstance|ALS_Pawn")
	void OnALSGaitChange();
	virtual void OnALSGaitChange_Implementation() override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AnimInstance|ALS_Pawn")
	void SetALSStance(const ELSStance NewALSStance);
	virtual void SetALSStance_Implementation(const ELSStance NewALSStance) override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AnimInstance|ALS_Pawn")
	void OnALSStanceChange();
	virtual void OnALSStanceChange_Implementation() override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AnimInstance|ALS_Pawn")
	void SetALSRotationMode(const ELSRotationMode NewALSRotationMode);
	virtual void SetALSRotationMode_Implementation(const ELSRotationMode NewALSRotationMode) override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AnimInstance|ALS_Pawn")
	void OnALSRotationModeChange();
	virtual void OnALSRotationModeChange_Implementation() override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AnimInstance|ALS_Pawn")
	void SetALSViewMode(const ELSViewMode NewALSViewMode);
	virtual void SetALSViewMode_Implementation(const ELSViewMode NewALSViewMode) override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AnimInstance|ALS_Pawn")
	void OnALSViewModeChange();
	virtual void OnALSViewModeChange_Implementation() override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AnimInstance|ALS_Pawn")
	void SetALSAiming(const bool NewALSAiming);
	virtual void SetALSAiming_Implementation(const bool NewALSAiming) override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AnimInstance|ALS_Pawn")
	void OnALSAimingChange();
	virtual void OnALSAimingChange_Implementation() override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AnimInstance|ALS_Pawn")
	void SetALSIdleState(const ELSIdleEntryState InLSIdleEntryState);
	virtual void SetALSIdleState_Implementation(const ELSIdleEntryState InLSIdleEntryState) override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AnimInstance|ALS_Pawn")
	void SetWalkingSpeed(const float InWalkingSpeed);
	virtual void SetWalkingSpeed_Implementation(const float InWalkingSpeed) override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AnimInstance|ALS_Pawn")
	void SetRunningSpeed(const float InRunningSpeed);
	virtual void SetRunningSpeed_Implementation(const float InRunningSpeed) override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AnimInstance|ALS_Pawn")
	void SetSprintingSpeed(const float InSprintingSpeed);
	virtual void SetSprintingSpeed_Implementation(const float InSprintingSpeed) override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AnimInstance|ALS_Pawn")
	void SetCrouchingSpeed(const float InCrouchingSpeed);
	virtual void SetCrouchingSpeed_Implementation(const float InCrouchingSpeed) override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AnimInstance|ALS_Pawn")
	void SetSwimmingSpeed(const float InSwimmingSpeed);
	virtual void SetSwimmingSpeed_Implementation(const float InSwimmingSpeed) override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AnimInstance|ALS_Pawn")
	float GetWalkingSpeed() const;
	virtual float GetWalkingSpeed_Implementation() const override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AnimInstance|ALS_Pawn")
	float GetRunningSpeed() const;
	virtual float GetRunningSpeed_Implementation() const override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AnimInstance|ALS_Pawn")
	float GetSprintingSpeed() const;
	virtual float GetSprintingSpeed_Implementation() const override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AnimInstance|ALS_Pawn")
	float GetCrouchingSpeed() const;
	virtual float GetCrouchingSpeed_Implementation() const override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AnimInstance|ALS_Pawn")
	float GetSwimmingSpeed() const;
	virtual float GetSwimmingSpeed_Implementation() const override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AnimInstance|ALS_Pawn")
	void SetGetup(const bool InFaceDown);
	virtual void SetGetup_Implementation(const bool InFaceDown) override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AnimInstance|ALS_Pawn")
	void SetRF(const bool InRF);
	virtual void SetRF_Implementation(const bool InRF) override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AnimInstance|ALS_Pawn")
	void PoseSnapShot(const FName InPoseName);
	virtual void PoseSnapShot_Implementation(const FName InPoseName) override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AnimInstance|ALS_Pawn")
	FCameraFOVParam GetCameraFOVParam() const;
	FCameraFOVParam GetCameraFOVParam_Implementation() const override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AnimInstance|ALS_Pawn")
	FCameraTraceParam GetCameraTraceParam() const;
	FCameraTraceParam GetCameraTraceParam_Implementation() const override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterBase|ALS_Pawn")
	bool HasDebugTrace() const;
	virtual bool HasDebugTrace_Implementation() const override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AnimInstance|ALS_AnimNotify")
	void SetALSAnimNotifyTurnInPlace(UAnimMontage* InTurnInPlaceMontage, const bool InShouldTurnInPlace, const bool InTurningInPlace, const bool InTurningRight);
	virtual void SetALSAnimNotifyTurnInPlace_Implementation(UAnimMontage* InTurnInPlaceMontage, const bool InShouldTurnInPlace, const bool InTurningInPlace, const bool InTurningRight) override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AnimInstance|ALS_AnimNotify")
	void SetALSAnimNotifyPivotData(const FPivotData InPivotData);
	virtual void SetALSAnimNotifyPivotData_Implementation(const FPivotData InPivotData) override;
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

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ALS")
	ELSMovementAction ALSMovementAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ALS")
	ELSGait ALSGait;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ALS")
	ELSStance ALSStance;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ALS")
	ELSRotationMode ALSRotationMode;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ALS")
	ELSMovementMode ALSPrevMovementMode;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ALS")
	ELSViewMode ALSViewMode;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ALS")
	ELSIdleEntryState ALSIdleEntryState;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ALS")
	FVector Velocity;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ALS")
	FVector2D AimOffset;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ALS")
	FVector2D LeanGrounded;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ALS")
	FVector2D FeetPosition;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ALS")
	FRotator CharacterRotation;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ALS")
	FRotator LastVelocityRotation;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ALS")
	FRotator LastMovementInputRotation;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ALS")
	FRotator LookingRotation;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ALS")
	FRotator PreviousVelocityRotation;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ALS")
	float Direction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ALS")
	float AimYawDelta;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ALS")
	float AimYawRate;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ALS")
	float GaitValue;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ALS")
	float N_PlayRate;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ALS")
	float C_PlayRate;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ALS")
	float LeanInAir;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ALS")
	float AccelerationDifference;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ALS")
	float PreviousSpeed;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ALS")
	float StartPosition;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ALS")
	float FlailRate;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ALS")
	float FlailBlendAlpha;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ALS")
	float LandPredictionAlpha;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ALS")
	float RotationDifference;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ALS")
	float VelocityDifference;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ALS")
	float TurnInPlaceDelayCount;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ALS")
	float DeltaVelocityDifference;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ALS")
	bool bWasMovementInput;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ALS")
	bool bTurningInPlace;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ALS")
	bool bTurningRight;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ALS")
	bool bShouldTurnInPlace;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ALS")
	bool bRF;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ALS")
	bool bDebugTrace;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ALS")
	ELSMovementDirection MovementDirection;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ALS")
	ELSLocomotionState ActiveLocomotionState;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ALS")
	class UAnimMontage* ActiveTurnInPlaceMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ALS")
	class UAnimSequence* AdditiveLand;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ALS")
	struct FPivotData PivotData;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ALS")
	class UCurveFloat* FlailAlphaCurve;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ALS")
	class UCurveFloat* LandAlphaCurve;
#pragma endregion

#pragma region ALSTurn
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ALS")
	struct FTurnMontages N_Turn_90;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ALS")
	struct FTurnMontages N_Turn_180;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ALS")
	struct FTurnMontages LF_Turn_90;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ALS")
	struct FTurnMontages RF_Turn_90;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ALS")
	struct FTurnMontages CLF_Turn_90;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ALS")
	struct FTurnMontages CRF_Turn_90;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ALS")
	struct FCombatTurnMontages CombatTurnData;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ALS")
	struct FCombatTurnMontages CombatCrouchTurnData;

	FTurnMontages GetCrouchTurnData() const
	{
		//switch (WeaponItemType)
		//{
		//	case EWeaponItemType::Pistol:
		//	return CombatCrouchTurnData.PistolTurnData;
		//	case EWeaponItemType::Rifle:
		//	return CombatCrouchTurnData.RifleTurnData;
		//	case EWeaponItemType::Sniper:
		//	return CombatCrouchTurnData.SniperRifleTurnData;
		//}
		return bRF ? CRF_Turn_90 : CLF_Turn_90;
	}

	FTurnMontages GetTurnData() const
	{
		//switch (WeaponItemType)
		//{
		//	case EWeaponItemType::Pistol:
		//	return CombatTurnData.PistolTurnData;
		//	case EWeaponItemType::Rifle:
		//	return CombatTurnData.RifleTurnData;
		//	case EWeaponItemType::Sniper:
		//	return CombatTurnData.SniperRifleTurnData;
		//}
		return bRF ? RF_Turn_90 : LF_Turn_90;
	}
#pragma endregion

#pragma region ALSNativeEvent
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "ALS|Event")
	void BP_ReplicatePlayMontage(UAnimMontage* MontageToPlay, const float InPlayRate, const float InTimeToStartMontageAt, const bool bStopAllMontages);
#pragma endregion

#pragma region ALSFunction
	void SetVariableFromOwner();
	void DoWhileGrounded();
	void DoWhileFalling();
	void DoWhileRagdoll();
	void DoWhileLocomotionState();

	void WhileMoving();
	void WhileTurnInPlace();

	void CalculateGaitValue();
	void CalculateMovementState();
	void CalculatePlayRates(const float WalkAnimSpeed, const float RunAnimSpeed, const float SprintAnimSpeed, const float CrouchAnimSpeed);
	void CalculateMovementDirection(const float DirectionThresholdMin, const float DirectionThresholdMax, const float Buffer);
	void CalculateInAirLeaningValues();
	void CalculateAimOffset();
	void CalculateStartPosition();
	void CalcuratePivotState();
	void CalculateGroundedLeaningValues();
	void CalculateLandPredictionAlpha();

	void OnTurnInPlaceRespons(const float AimYawLimit, const FTurnMontages TurnAnims, const float PlayRate);
	void OnTurnInPlaceDelay(
		const float MaxCameraSpeed,
		const float AimYawLimitFirst,
		const float DelayTimeFirst,
		const float PlayRateFirst,
		const FTurnMontages TurnAnimsFirst,
		const float AimYawLimitSecond,
		const float DelayTimeSecond,
		const float PlayRateSecond,
		const FTurnMontages TurnAnimsSecond);

	void UpdateMovementSpeed(const bool bWasGround);
	void UpdateFlailBlendAlpha();


	UFUNCTION(BlueprintCallable, Category = "ALS")
	void IdleTransition(UAnimSequenceBase* Animation, const float InPlayRate, const float InTimeToStartMontageAt);
#pragma endregion

// ActionInfo Structs. Reference AnimMontage Pointer
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Combat")
	TArray<FWeaponActionInfo> ActionInfoArray;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Combat")
	TArray<FCharacterComboInfo> ComboInfoArray;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Combat")
	class UAnimMontage* DefaultHitDamageMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "MantleSystem")
	FMantleAsset DefaultLowMantleAsset;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "MantleSystem")
	FMantleAsset DefaultHighMantleAsset;

public:
	FWeaponActionInfo* GetActionInfo(const EWeaponItemType InWeaponItemType);

	FMantleAsset GetMantleAsset(const EMantleType InMantleType) const;

	const float TakeDefaultDamageAnimation(FWeaponActionInfo* const InActionInfoPtr, const FName InSlotNodeName);

	const float PlayAnimationSequence(const FAnimSequenceInfo InAnimSequenceInfo, const FName InSlotNodeName);

protected:
	const float TakeDefaultDamage();
};
