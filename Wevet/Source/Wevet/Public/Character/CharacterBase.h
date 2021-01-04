// Copyright 2018 wevet works All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Curves/CurveFloat.h"
#include "Curves/CurveVector.h"
#include "Wevet.h"
#include "WevetTypes.h"
#include "CharacterModel.h"

// Weapon
#include "Weapon/AbstractWeapon.h"

// Item
#include "Item/AbstractItem.h"

// Interface
#include "Interface/DamageInstigator.h"
#include "Interface/AttackInstigator.h"
#include "Interface/SoundInstigator.h"
#include "Interface/InteractionPawn.h"

// ActionInfo
#include "Structs/WeaponActionInfo.h"
#include "Structs/CharacterComboInfo.h"

// EngineComponent
#include "Components/TimelineComponent.h"

// Components
#include "Component/CharacterInventoryComponent.h"
#include "Component/CharacterPickupComponent.h"
#include "Component/ComboComponent.h"

#include "Perception/AISightTargetInterface.h"
#include "GenericTeamAgentInterface.h"

// Plugins Locomotion
#include "LocomotionSystemTypes.h"
#include "LocomotionSystemStructs.h"
#include "LocomotionSystemPawn.h"
#include "CharacterBase.generated.h"

using WeaponFunc = TFunction<void(AAbstractWeapon* Weapon)>;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FDeathDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FKillDelegate, AActor* const, InActor);
class UCharacterAnimInstanceBase;
class UIKAnimInstance;

UCLASS(ABSTRACT)
class WEVET_API ACharacterBase : public ACharacter, 
	public IDamageInstigator, 
	public IAttackInstigator, 
	public ISoundInstigator, 
	public IInteractionPawn,
	public ILocomotionSystemPawn,
	public IAISightTargetInterface, 
	public IGenericTeamAgentInterface
{
	GENERATED_BODY()

public:
	ACharacterBase(const FObjectInitializer& ObjectInitializer);
	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void BeginDestroy() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void Tick(float DeltaTime) override;
	virtual float TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

protected:
	virtual void BeginPlay() override;
	virtual void OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode = 0) override;
	virtual void OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;
	virtual void OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;

public:
	UPROPERTY(BlueprintAssignable)
	FDeathDelegate DeathDelegate;

	UPROPERTY(BlueprintAssignable)
	FKillDelegate KillDelegate;

public:
	virtual void Jump() override;
	virtual void StopJumping() override;

	UFUNCTION(BlueprintCallable, Category = "CharacterBase|Function")
	virtual void Sprint();

	UFUNCTION(BlueprintCallable, Category = "CharacterBase|Function")
	virtual void StopSprint();

	/**
	* Retrieve team identifier in form of FGenericTeamId
	* Returns the FGenericTeamID that represents "which team this character belongs to".
	* There are three teams prepared by default: hostile, neutral, and friendly.
	* Required for AI Perception's "Detection by Affiliation" to work.
	*/
	virtual FGenericTeamId GetGenericTeamId() const override;

	/**	
	* Implementation should check whether from given ObserverLocation
	* implementer can be seen. If so OutSeenLocation should contain
	* first visible location
	* Return sight strength for how well the target is seen.
	*/
	virtual bool CanBeSeenFrom(const FVector& ObserverLocation, FVector& OutSeenLocation, int32& NumberOfLoSChecksPerformed, float& OutSightStrength, const AActor* IgnoreActor = nullptr) const override;

protected:
	virtual void TurnAtRate(float Rate);
	virtual void LookUpAtRate(float Rate);
	virtual void MoveForward(float Value);
	virtual void MoveRight(float Value);

	UFUNCTION(BlueprintCallable, Category = "CharacterBase|Function")
	virtual void OnCrouch();

	UFUNCTION(BlueprintCallable, Category = "CharacterBase|Function")
	virtual void CrouchUpdate(const bool InCrouch);

	virtual void OnCrouchUpdate();

public:
#pragma region InteractionPawn
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterBase|InteractionPawn")
	void Pickup(const EItemType InItemType, AActor* Actor);
	virtual void Pickup_Implementation(const EItemType InItemType, AActor* Actor) override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterBase|InteractionPawn")
	bool CanPickup() const;
	virtual bool CanPickup_Implementation() const override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterBase|InteractionPawn")
	void Release();
	virtual void Release_Implementation() override;
#pragma endregion

#pragma region SoundInstigator
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterBase|SoundInstigator")
	void ReportNoise(USoundBase* Sound, float Volume);
	virtual void ReportNoise_Implementation(USoundBase* Sound, float Volume) override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterBase|SoundInstigator")
	void FootStep(USoundBase* Sound, float Volume);
	virtual void FootStep_Implementation(USoundBase* Sound, float Volume) override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterBase|SoundInstigator")
	void ReportNoiseOther(AActor* Actor, USoundBase* Sound, const float Volume, const FVector Location);
	virtual void ReportNoiseOther_Implementation(AActor* Actor, USoundBase* Sound, const float Volume, const FVector Location) override;
#pragma endregion

#pragma region DamageInstigator
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterBase|IDamageInstigator")
	float MakeDamage(UCharacterModel* DamageModel, const int InWeaponDamage) const;
	virtual float MakeDamage_Implementation(UCharacterModel* DamageModel, const int InWeaponDamage) const override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterBase|IDamageInstigator")
	bool IsDeath() const;
	virtual bool IsDeath_Implementation() const override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterBase|IDamageInstigator")
	void Die();
	virtual void Die_Implementation() override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterBase|IDamageInstigator")
	void Alive();
	virtual void Alive_Implementation() override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterBase|IDamageInstigator")
	void InfrictionDamage(AActor* InfrictionActor, const bool bInfrictionDie);
	virtual void InfrictionDamage_Implementation(AActor* InfrictionActor, const bool bInfrictionDie) override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterBase|IDamageInstigator")
	void Equipment();
	virtual void Equipment_Implementation() override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterBase|IDamageInstigator")
	void UnEquipment();
	virtual void UnEquipment_Implementation() override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterBase|IDamageInstigator")
	UCharacterModel* GetPropertyModel() const;
	virtual UCharacterModel* GetPropertyModel_Implementation() const override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterBase|IDamageInstigator")
	bool CanKillDealDamage(const FName BoneName) const;
	virtual bool CanKillDealDamage_Implementation(const FName BoneName) const override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterBase|IDamageInstigator")
	void HitEffectReceive(const FHitResult& HitResult, const EGiveDamageType InGiveDamageType);
	virtual void HitEffectReceive_Implementation(const FHitResult& HitResult, const EGiveDamageType InGiveDamageType) override;
#pragma endregion

#pragma region AttackInstigator
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterBase|AttackInstigator")
	void DoFirePressed();
	virtual void DoFirePressed_Implementation() override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterBase|AttackInstigator")
	void DoFireReleassed();
	virtual void DoFireReleassed_Implementation() override;	

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterBase|AttackInstigator")
	void DoMeleeAttack();
	virtual void DoMeleeAttack_Implementation() override;	

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterBase|AttackInstigator")
	void DoReload();
	virtual void DoReload_Implementation() override;
#pragma endregion

public:
	FORCEINLINE class UAudioComponent* GetAudioComponent() const
	{
		return AudioComponent; 
	}

	FORCEINLINE class UCharacterPickupComponent* GetPickupComponent() const 
	{
		return PickupComponent; 
	}

	FORCEINLINE class UCharacterInventoryComponent* GetInventoryComponent() const
	{
		return InventoryComponent; 
	}

	FORCEINLINE class UComboComponent* GetComboComponent() const 
	{
		return ComboComponent; 
	}

public:
	UFUNCTION(BlueprintCallable, Category = "CharacterBase|Function")
	class UCharacterAnimInstanceBase* GetAnimInstance() const;
	class UIKAnimInstance* GetIKAnimInstance() const;

	FORCEINLINE bool HasCrouch() const 
	{
		return bCrouch; 
	}

	FORCEINLINE bool HasSprint() const 
	{
		return bSprint; 
	}

	FORCEINLINE bool WasStanning() const 
	{
		return bWasStanning; 
	}

	float GetHealthToWidget() const 
	{
		return CharacterModel->GetHealthToWidget(); 
	}

	bool IsFullHealth() const 
	{
		return CharacterModel->IsFullHealth(); 
	}
	
	bool IsHealthHalf() const 
	{
		return CharacterModel->IsHealthHalf(); 
	}
	
	bool IsHealthQuarter() const 
	{
		return CharacterModel->IsHealthQuarter(); 
	}

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components, meta = (AllowPrivateAccess = "true"))
	class UPawnNoiseEmitterComponent* PawnNoiseEmitterComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components, meta = (AllowPrivateAccess = "true"))
	class UAudioComponent* AudioComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components, meta = (AllowPrivateAccess = "true"))
	class UCharacterPickupComponent* PickupComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components, meta = (AllowPrivateAccess = "true"))
	class UCharacterInventoryComponent* InventoryComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components, meta = (AllowPrivateAccess = "true"))
	class UComboComponent* ComboComponent;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "CharacterBase|Combat")
	TSubclassOf<class AAbstractWeapon> PrimaryWeapon;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "CharacterBase|Combat")
	TSubclassOf<class AAbstractWeapon> SecondaryWeapon;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "CharacterBase|Combat")
	class UParticleSystem* BloodTemplate;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "CharacterBase|Variable")
	struct FGenericTeamId TeamId;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "CharacterBase|Variable")
	class USoundBase* FootStepSoundAsset;

	bool bCrouch;
	bool bWasDied;
	bool bWasStanning;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "CharacterBase|Variable")
	bool bSprint;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "CharacterBase|Variable")
	float BaseTurnRate;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "CharacterBase|Variable")
	float BaseLookUpRate;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "CharacterBase|Variable")
	float ForwardAxisValue;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "CharacterBase|Variable")
	float RightAxisValue;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "CharacterBase|Variable")
	FName HeadSocketName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "CharacterBase|Variable")
	FName HeadBoneName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "CharacterBase|Variable")
	FName PelvisSocketName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "CharacterBase|Variable")
	FName PelvisBoneName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "CharacterBase|Variable")
	FName ChestSocketName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "CharacterBase|Variable")
	FName ChestBoneName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "CharacterBase|Variable")
	bool bEnableRagdoll;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "CharacterBase|Variable")
	bool bDebugTrace;

protected:
	UPROPERTY(EditAnywhere, Instanced, Category = "CharacterBase|CharacterModel")
	UCharacterModel* CharacterModel;

	FWeaponActionInfo* ActionInfoPtr;

	/* damage motion timeout */
	float TakeDamageTimeOut;

	/* melee attack motion timeout */
	float MeleeAttackTimeOut;

	/* for ai equip weapon timeout */
	float EquipWeaponTimeOut;

	float StanTimeOut;
	FTimerHandle MeleeAttackHundle;
	FTimerHandle StanHundle;
	EGiveDamageType GiveDamageType;
	TArray<class AActor*> IgnoreActors;

public:
	UFUNCTION(BlueprintCallable, Category = "CharacterBase|Function")
	bool WasMeleeAttackPlaying() const { return MeleeAttackTimeOut >= ZERO_VALUE; }

	UFUNCTION(BlueprintCallable, Category = "CharacterBase|Function")
	bool WasTakeDamagePlaying() const { return TakeDamageTimeOut >= ZERO_VALUE; }

	UFUNCTION(BlueprintCallable, Category = "CharacterBase|Function")
	bool WasEquipWeaponPlaying() const { return EquipWeaponTimeOut >= ZERO_VALUE; }

	void SetStanning(const bool bInStanning)
	{
		bWasStanning = bInStanning;
	}

protected:
#pragma region WeaponProtected
	AAbstractWeapon* FindByWeapon(const EWeaponItemType WeaponItemType) const;

	/* pickup before had same weaponList */
	const bool WasSameWeaponType(AAbstractWeapon* const Weapon);

	/* pickup actor */
	virtual void PickupObjects();

	/* release actor */
	virtual void ReleaseObjects();
	virtual void TakeDamageMontage(const bool InForcePlaying);
	TWeakObjectPtr<class AAbstractWeapon> CurrentWeapon;

#pragma endregion

public:
#pragma region WeaponPublic
	bool HasEquipWeapon() const;

	UFUNCTION(BlueprintCallable, Category = "CharacterBase|Combat")
	class AAbstractWeapon* GetSelectedWeapon() const
	{
		if (CurrentWeapon.IsValid())
		{
			return CurrentWeapon.Get();
		}
		return nullptr;
	}

	virtual void EquipmentActionMontage();
	virtual void UnEquipmentActionMontage();
	virtual void FireActionMontage();
	virtual void ReloadActionMontage(float &OutReloadDuration);
	virtual void MeleeAttackMontage();

	virtual void MeleeAttack(const bool InEnable);
	virtual void ReleaseWeaponToWorld(const FTransform& Transform, AAbstractWeapon*& Weapon);
	virtual void CreateWeaponInstance(const TSubclassOf<class AAbstractWeapon> InWeaponTemplate, WeaponFunc Callback = nullptr);
	EWeaponItemType GetCurrentWeaponType() const;

	virtual void ReleaseAllWeaponInventory();
#pragma endregion

public:
#pragma region Utils
	virtual FVector BulletTraceRelativeLocation() const;
	virtual FVector BulletTraceForwardLocation() const;
	FVector GetHeadSocketLocation() const;
	FVector GetChestSocketLocation() const;

	UFUNCTION(BlueprintCallable, Category = "CharacterBase|Utils")
	FORCEINLINE EDrawDebugTrace::Type GetDrawDebugTrace() const
	{
		return bDebugTrace ? EDrawDebugTrace::Type::ForOneFrame : EDrawDebugTrace::Type::None;
	}

	UFUNCTION(BlueprintCallable, Category = "CharacterBase|Utils")
	FTransform GetChestTransform() const
	{
		if (GetMesh())
		{
			return GetMesh()->GetSocketTransform(ChestSocketName);
		}
		else
		{
			return FTransform::Identity;
		}
	}
#pragma endregion

	virtual void OverlapActor(AActor* InActor)
	{
		if (PickupComponent)
		{
			PickupComponent->SetPickupActor(InActor);
		}
	}

	const TArray<class AActor*>& GetIgnoreActors() 
	{
		return IgnoreActors; 
	}

protected:
	void SetActionInfo(const EWeaponItemType InWeaponItemType);

	UFUNCTION()
	virtual void HitReceive(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
	void KillRagdollPhysics();

	UFUNCTION(BlueprintCallable, Category = "CharacterBase|Utils")
	void SetForwardOrRightVector(FVector& OutForwardVector, FVector& OutRightVector);

	UFUNCTION(BlueprintCallable, Category = "CharacterBase|Ragdoll")
	virtual void StartRagdollAction();

	UFUNCTION(BlueprintCallable, Category = "CharacterBase|Ragdoll")
	virtual void RagdollToWakeUpAction();
	float RagdollTimer;
	float RagdollInterval;

	ELSMovementMode GetPawnMovementModeChanged(const EMovementMode PrevMovementMode, const uint8 PrevCustomMode) const;
	void ConvertALSMovementMode();

#pragma region ALS
public:
	UPROPERTY(Replicated, BlueprintReadWrite)
	FVector MovementInput;

	UPROPERTY(Replicated, BlueprintReadWrite)
	FVector RagdollLocation;

	UPROPERTY(Replicated, BlueprintReadWrite)
	FRotator CharacterRotation;

	UPROPERTY(Replicated, BlueprintReadWrite)
	FRotator LookingRotation;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "CharacterBase|ALS")
	struct FCameraFOVParam CameraFOVParam;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "CharacterBase|ALS")
	bool bWasMoving;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "CharacterBase|ALS")
	bool bWasMovementInput;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "CharacterBase|ALS")
	bool bRagdollOnGround;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "CharacterBase|ALS")
	bool bAiming;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "CharacterBase|ALS")
	float RotationRateMultiplier;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "CharacterBase|ALS")
	float RotationOffset;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "CharacterBase|ALS")
	float WalkingSpeed;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "CharacterBase|ALS")
	float RunningSpeed;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "CharacterBase|ALS")
	float SprintingSpeed;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "CharacterBase|ALS")
	float CrouchingSpeed;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "CharacterBase|ALS")
	float SwimmingSpeed;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "CharacterBase|ALS")
	float WalkingAcceleration;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "CharacterBase|ALS")
	float RunningAcceleration;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "CharacterBase|ALS")
	float WalkingDeceleration;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "CharacterBase|ALS")
	float RunningDeceleration;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "CharacterBase|ALS")
	float WalkingGroundFriction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "CharacterBase|ALS")
	float RunningGroundFriction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "CharacterBase|ALS")
	float AimYawDelta;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "CharacterBase|ALS")
	float AimYawRate;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "CharacterBase|ALS")
	float VelocityDifference;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "CharacterBase|ALS")
	float RotationDifference;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "CharacterBase|ALS")
	float Direction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "CharacterBase|ALS")
	FVector RagdollVelocity;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "CharacterBase|ALS")
	FRotator JumpRotation;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "CharacterBase|ALS")
	FRotator LastVelocityRotation;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "CharacterBase|ALS")
	FRotator LastMovementInputRotation;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "CharacterBase|ALS")
	FRotator TargetRotation;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "CharacterBase|ALS")
	ELSMovementMode ALSMovementMode;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "CharacterBase|ALS")
	ELSMovementMode ALSPrevMovementMode;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "CharacterBase|ALS")
	ELSMovementAction ALSMovementAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "CharacterBase|ALS")
	ELSMovementAction ALSPrevMovementAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "CharacterBase|ALS")
	ELSGait ALSGait;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "CharacterBase|ALS")
	ELSStance ALSStance;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "CharacterBase|ALS")
	ELSViewMode ALSViewMode;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "CharacterBase|ALS")
	ELSRotationMode ALSRotationMode;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "CharacterBase|ALS")
	ELSCardinalDirection CardinalDirection;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "CharacterBase|ALS")
	FName RagdollPoseSnapshot;

#pragma endregion

#pragma region ALSInterface
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterBase|ALS")
	ELSMovementMode GetALSMovementMode() const;
	virtual ELSMovementMode GetALSMovementMode_Implementation() const override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterBase|ALS")
	ELSMovementAction GetALSMovementAction() const;
	virtual ELSMovementAction GetALSMovementAction_Implementation() const override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterBase|ALS")
	ELSGait GetALSGait() const;
	virtual ELSGait GetALSGait_Implementation() const override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterBase|ALS")
	ELSStance GetALSStance() const;
	virtual ELSStance GetALSStance_Implementation() const override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterBase|ALS")
	ELSViewMode GetALSViewMode() const;
	virtual ELSViewMode GetALSViewMode_Implementation() const override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterBase|ALS")
	ELSRotationMode GetALSRotationMode() const;
	virtual ELSRotationMode GetALSRotationMode_Implementation() const override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterBase|ALS")
	bool HasMovementInput() const;
	virtual bool HasMovementInput_Implementation() const override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterBase|ALS")
	bool HasAiming() const;
	virtual bool HasAiming_Implementation() const override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterBase|ALS")
	FTransform GetPivotTarget() const;
	virtual FTransform GetPivotTarget_Implementation() const override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterBase|ALS")
	FVector GetCameraTarget() const;
	virtual FVector GetCameraTarget_Implementation() const override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterBase|ALS_Pawn")
	void SetALSCharacterRotation(const FRotator AddAmount);
	virtual void SetALSCharacterRotation_Implementation(const FRotator AddAmount) override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterBase|ALS_Pawn")
	void SetALSCameraShake(TSubclassOf<class UMatineeCameraShake> InShakeClass, const float InScale);
	virtual void SetALSCameraShake_Implementation(TSubclassOf<class UMatineeCameraShake> InShakeClass, const float InScale) override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterBase|ALS_Pawn")
	void SetALSMovementMode(const ELSMovementMode NewALSMovementMode);
	virtual void SetALSMovementMode_Implementation(const ELSMovementMode NewALSMovementMode) override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterBase|ALS_Pawn")
	void OnALSMovementModeChange();
	virtual void OnALSMovementModeChange_Implementation() override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterBase|ALS_Pawn")
	void SetALSMovementAction(const ELSMovementAction NewALSMovementAction);
	virtual void SetALSMovementAction_Implementation(const ELSMovementAction NewALSMovementAction) override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterBase|ALS_Pawn")
	void OnALSMovementActionChange();
	virtual void OnALSMovementActionChange_Implementation() override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterBase|ALS_Pawn")
	void SetALSGait(const ELSGait NewALSGait);
	virtual void SetALSGait_Implementation(const ELSGait NewALSGait) override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterBase|ALS_Pawn")
	void OnALSGaitChange();
	virtual void OnALSGaitChange_Implementation() override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterBase|ALS_Pawn")
	void SetALSStance(const ELSStance NewALSStance);
	virtual void SetALSStance_Implementation(const ELSStance NewALSStance) override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterBase|ALS_Pawn")
	void OnALSStanceChange();
	virtual void OnALSStanceChange_Implementation() override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterBase|ALS_Pawn")
	void SetALSRotationMode(const ELSRotationMode NewALSRotationMode);
	virtual void SetALSRotationMode_Implementation(const ELSRotationMode NewALSRotationMode) override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterBase|ALS_Pawn")
	void OnALSRotationModeChange();
	virtual void OnALSRotationModeChange_Implementation() override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterBase|ALS_Pawn")
	void SetALSViewMode(const ELSViewMode NewALSViewMode);
	virtual void SetALSViewMode_Implementation(const ELSViewMode NewALSViewMode) override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterBase|ALS_Pawn")
	void OnALSViewModeChange();
	virtual void OnALSViewModeChange_Implementation() override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterBase|ALS_Pawn")
	void SetALSAiming(const bool NewALSAiming);
	virtual void SetALSAiming_Implementation(const bool NewALSAiming) override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterBase|ALS_Pawn")
	void OnALSAimingChange();
	virtual void OnALSAimingChange_Implementation() override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterBase|ALS_Pawn")
	void SetALSIdleState(const ELSIdleEntryState InLSIdleEntryState);
	virtual void SetALSIdleState_Implementation(const ELSIdleEntryState InLSIdleEntryState) override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterBase|ALS_Pawn")
	void SetWalkingSpeed(const float InWalkingSpeed);
	virtual void SetWalkingSpeed_Implementation(const float InWalkingSpeed) override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterBase|ALS_Pawn")
	void SetRunningSpeed(const float InRunningSpeed);
	virtual void SetRunningSpeed_Implementation(const float InRunningSpeed) override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterBase|ALS_Pawn")
	void SetSprintingSpeed(const float InSprintingSpeed);
	virtual void SetSprintingSpeed_Implementation(const float InSprintingSpeed) override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterBase|ALS_Pawn")
	void SetCrouchingSpeed(const float InCrouchingSpeed);
	virtual void SetCrouchingSpeed_Implementation(const float InCrouchingSpeed) override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterBase|ALS_Pawn")
	void SetSwimmingSpeed(const float InSwimmingSpeed);
	virtual void SetSwimmingSpeed_Implementation(const float InSwimmingSpeed) override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterBase|ALS_Pawn")
	float GetWalkingSpeed() const;
	virtual float GetWalkingSpeed_Implementation() const override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterBase|ALS_Pawn")
	float GetRunningSpeed() const;
	virtual float GetRunningSpeed_Implementation() const override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterBase|ALS_Pawn")
	float GetSprintingSpeed() const;
	virtual float GetSprintingSpeed_Implementation() const override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterBase|ALS_Pawn")
	float GetCrouchingSpeed() const;
	virtual float GetCrouchingSpeed_Implementation() const override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterBase|ALS_Pawn")
	float GetSwimmingSpeed() const;
	virtual float GetSwimmingSpeed_Implementation() const override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterBase|ALS_Pawn")
	void SetGetup(const bool InFaceDown);
	virtual void SetGetup_Implementation(const bool InFaceDown) override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterBase|ALS_Pawn")
	void SetRF(const bool InRF);
	virtual void SetRF_Implementation(const bool InRF) override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterBase|ALS_Pawn")
	void PoseSnapShot(const FName InPoseName);
	virtual void PoseSnapShot_Implementation(const FName InPoseName) override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterBase|ALS_Pawn")
	FCameraFOVParam GetCameraFOVParam() const;
	FCameraFOVParam GetCameraFOVParam_Implementation() const override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterBase|ALS_Pawn")
	FCameraTraceParam GetCameraTraceParam() const;
	FCameraTraceParam GetCameraTraceParam_Implementation() const override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterBase|ALS_Pawn")
	bool HasDebugTrace() const;
	virtual bool HasDebugTrace_Implementation() const override;
#pragma endregion

public:
	virtual FVector ChooseVelocity() const
	{
		if (ALSMovementMode == ELSMovementMode::Ragdoll)
		{
			return GetMesh()->GetPhysicsLinearVelocity(PelvisBoneName);
		}
		return Super::GetVelocity();
	}

	FORCEINLINE bool HasMoving() const
	{ 
		return bWasMoving;
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

	FORCEINLINE float GetAimYawDelta() const 
	{
		return AimYawDelta; 
	}

	FORCEINLINE float GetAimYawRate() const 
	{
		return AimYawRate; 
	}

	FORCEINLINE float GetVelocityDifference() const 
	{
		return VelocityDifference; 
	}

	FORCEINLINE float GetRotationDifference() const 
	{ 
		return RotationDifference;
	}

	FORCEINLINE float GetDirection() const 
	{ 
		return Direction; 
	}

	FORCEINLINE FRotator GetLastVelocityRotation() const 
	{ 
		return LastVelocityRotation; 
	}

	FORCEINLINE FRotator GetLastMovementInputRotation() const 
	{ 
		return LastMovementInputRotation; 
	}

	float ChooseMaxWalkSpeed() const;

protected:
	UFUNCTION(BlueprintCallable, Category = "CharacterBase|ALS")
	void ManageCharacterRotation();

	UFUNCTION(BlueprintCallable, Category = "CharacterBase|ALS")
	void DoCharacterFalling();

	UFUNCTION(BlueprintCallable, Category = "CharacterBase|ALS")
	void DoCharacterGrounded();

	UFUNCTION(BlueprintCallable, Category = "CharacterBase|ALS")
	void DoWhileGrounded();

	UFUNCTION(BlueprintCallable, Category = "CharacterBase|ALS")
	bool CanSprint() const;

	UFUNCTION(BlueprintCallable, Category = "CharacterBase|ALS")
	void AddCharacterRotation(const FRotator AddAmount);

	UFUNCTION(BlueprintCallable, Category = "CharacterBase|ALS")
	void UpdateCharacterMovementSettings();

	UFUNCTION(BlueprintCallable, Category = "CharacterBase|ALS")
	void DoWhileRagdoll(FRotator& OutActorRotation, FVector& OutActorLocation);

	void ApplyCharacterRotation(const FRotator InTargetRotation, const bool bInterpRotation, const float InterpSpeed);
	void LimitRotation(const float AimYawLimit, const float InterpSpeed);
	bool CardinalDirectionAngles(const float Value, const float Min, const float Max, const float Buffer, const ELSCardinalDirection InCardinalDirection) const;
	void CustomAcceleration();

	void CalculateActorTransformRagdoll(const FRotator InRagdollRotation, const FVector InRagdollLocation, FRotator& OutActorRotation, FVector& OutActorLocation);
	void CalculateEssentialVariables();
	const float CalculateRotationRate(const float SlowSpeed, const float SlowSpeedRate, const float FastSpeed, const float FastSpeedRate);
	const FRotator LookingDirectionWithOffset(const float OffsetInterpSpeed, const float NEAngle, const float NWAngle, const float SEAngle, const float SWAngle, const float Buffer);


#pragma region MantleSystem
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "CharacterBase|MantleSystem")
	FMantleParams MantleParams;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "CharacterBase|MantleSystem")
	FLSComponentAndTransform MantleLedgeLS;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "CharacterBase|MantleSystem")
	FTransform MantleTarget;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "CharacterBase|MantleSystem")
	FTransform MantleActualStartOffset;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "CharacterBase|MantleSystem")
	FTransform MantleAnimatedStartOffset;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "CharacterBase|MantleSystem")
	FMantleTraceSettings FallingTraceSettings;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "CharacterBase|MantleSystem")
	FMantleTraceSettings GroundedTraceSettings;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "CharacterBase|MantleSystem")
	FMantleTraceSettings AutomaticTraceSettings;

protected:
	UFUNCTION(BlueprintCallable, Category = "CharacterBase|MantleSystem")
	FVector GetCapsuleLocationFromBase(const FVector BaseLocation, const float ZOffset) const;

	UFUNCTION(BlueprintCallable, Category = "CharacterBase|MantleSystem")
	FVector GetCapsuleBaseLocation(const float ZOffset) const;

	UFUNCTION(BlueprintCallable, Category = "CharacterBase|MantleSystem")
	const FVector GetPlayerMovementInput();

	UFUNCTION(BlueprintCallable, Category = "CharacterBase|MantleSystem")
	bool CapsuleHasRoomCheck(const FVector TargetLocation, const float HeightOffset, const float RadiusOffset) const;

public:
	FORCEINLINE FMantleTraceSettings GetFallingTraceSettings() const { return FallingTraceSettings; }
	FORCEINLINE FMantleTraceSettings GetGroundedTraceSettings() const { return GroundedTraceSettings; }
	FORCEINLINE FMantleTraceSettings GetAutomaticTraceSettings() const { return AutomaticTraceSettings; }

	UFUNCTION(BlueprintCallable, Category = "CharacterBase|MantleSystem")
	const bool MantleCheck(const FMantleTraceSettings InTraceSetting);

	UFUNCTION(BlueprintCallable, Category = "CharacterBase|MantleSystem")
	void MantleEnd();

	UFUNCTION(BlueprintCallable, Category = "CharacterBase|MantleSystem")
	void SetActorLocationAndRotation(const FVector NewLocation, const FRotator NewRotation, const bool bWasSweep, const bool bWasTeleport);

protected:
	EMantleType GetMantleType(const float InMantleHeight) const;
	void TraceForwardToFindWall(const FMantleTraceSettings InTraceSetting, FVector& OutInitialTraceImpactPoint, FVector& OutInitialTraceNormal, bool& OutHitResult);
	void SphereTraceByMantleCheck(const FMantleTraceSettings TraceSetting, const FVector InitialTraceImpactPoint, const FVector InitialTraceNormal, bool &OutHitResult, FVector &OutDownTraceLocation, UPrimitiveComponent* &OutPrimitiveComponent);
	void ConvertMantleHeight(const FVector DownTraceLocation, const FVector InitialTraceNormal, bool &OutRoomCheck, FTransform &OutTargetTransform, float &OutMantleHeight);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "CharacterBase|MantleSystem")
	void BP_MantleStart(const float MantleHeight, const FLSComponentAndTransform MantleLedgeWS, const EMantleType MantleType);
#pragma endregion


#pragma region MantleUpdate
public:
	UFUNCTION(BlueprintCallable, Category = "CharacterBase|MantleSystem")
	void MantleUpdate(const float BlendIn, const float InPlaybackPosition);

	UFUNCTION(BlueprintCallable, Category = "CharacterBase|MantleSystem")
	void SetMantleTarget();

protected:
	void SetMantleUpdateAlpha(const float InPlaybackPosition, float& OutPositionAlpha, float& OutXYCorrectionAlpha, float& OutZCorrectionAlpha);
	FTransform MakeXYCollectionAlphaTransform(const float InXYCollectionAlpha) const;
	FTransform MakeZCollectionAlphaTransform(const float InZCollectionAlpha) const;
	FTransform MakeMantleTransform(const float InPositionAlpha, const float InXYCollectionAlpha, const float InZCollectionAlpha) const;
	FTransform MakeMantleLerpedTarget(const float BlendIn, const float InPositionAlpha, const float InXYCollectionAlpha, const float InZCollectionAlpha) const;
#pragma endregion


#pragma region MantleStart
public:
	UFUNCTION(BlueprintCallable, Category = "CharacterBase|MantleSystem")
	void MantleStart(const float InMantleHeight, const FLSComponentAndTransform MantleLedgeWorldSpace, const EMantleType InMantleType, UTimelineComponent* const InMantleTimeline);

protected:
	void SetMantleParams(FMantleAsset InMantleAsset, const float InMantleHeight);
	void SetMantleLedgeLocalSpace(const FLSComponentAndTransform MantleLedgeWorldSpace);
	void CalculateMantleTarget(const FLSComponentAndTransform MantleLedgeWorldSpace);
	void CalculateMantleAnimatedStartOffset();
	void PlayFromStartMantle(UTimelineComponent* const InMantleTimeline);
#pragma endregion


};
