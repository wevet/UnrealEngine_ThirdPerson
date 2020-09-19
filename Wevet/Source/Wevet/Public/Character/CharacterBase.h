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

// Plugins Locomotion
#include "LocomotionSystemTypes.h"
#include "LocomotionSystemStructs.h"
#include "LocomotionSystemPropertyGetter.h"
#include "CharacterBase.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FDeathDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FKillDelegate, AActor* const, InActor);
class UCharacterAnimInstanceBase;
class UIKAnimInstance;

UCLASS(ABSTRACT)
class WEVET_API ACharacterBase : public ACharacter, 
	public IDamageInstigator, 
	public ISoundInstigator, 
	public IInteractionPawn,
	public ILocomotionSystemPropertyGetter
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

public:
	UPROPERTY(BlueprintAssignable)
	FDeathDelegate DeathDelegate;

	UPROPERTY(BlueprintAssignable)
	FKillDelegate KillDelegate;

public:
	virtual void Jump() override;
	virtual void StopJumping() override;
	virtual void Sprint();
	virtual void StopSprint();

protected:
	virtual void TurnAtRate(float Rate);
	virtual void LookUpAtRate(float Rate);
	virtual void MoveForward(float Value);
	virtual void MoveRight(float Value);

	UFUNCTION(BlueprintCallable, Category = "CharacterBase|Function")
	virtual void OnCrouch();

public:
#pragma region InteractionPawn
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterBase|InteractionPawn")
	void Pickup(const EItemType InItemType, AActor* Actor);
	virtual void Pickup_Implementation(const EItemType InItemType, AActor* Actor) override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterBase|InteractionPawn")
	const bool CanPickup();
	virtual const bool CanPickup_Implementation() override;

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
	void MakeDamage(UCharacterModel* DamageModel, const int InWeaponDamage, float& OutDamage);
	virtual void MakeDamage_Implementation(UCharacterModel* DamageModel, const int InWeaponDamage, float& OutDamage) override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterBase|IDamageInstigator")
	bool IsDeath() const;
	virtual bool IsDeath_Implementation() const override
	{
		if (bWasDied || !CharacterModel)
		{
			return true;
		}
		return CharacterModel->IsDie();
	}

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
	virtual UCharacterModel* GetPropertyModel_Implementation() const override
	{
		return CharacterModel;
	}

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterBase|IDamageInstigator")
	bool CanKillDealDamage(const FName BoneName) const;
	virtual bool CanKillDealDamage_Implementation(const FName BoneName) const override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterBase|IDamageInstigator")
	void HitEffectReceive(const FHitResult& HitResult, const EGiveDamageType InGiveDamageType);
	virtual void HitEffectReceive_Implementation(const FHitResult& HitResult, const EGiveDamageType InGiveDamageType) override;
#pragma endregion

public:
	FORCEINLINE class UAudioComponent* GetAudioComponent() const { return AudioComponent; }
	FORCEINLINE class UCharacterPickupComponent* GetPickupComponent() const { return PickupComponent; }
	FORCEINLINE class UCharacterInventoryComponent* GetInventoryComponent() const { return InventoryComponent; }
	FORCEINLINE class UComboComponent* GetComboComponent() const { return ComboComponent; }

public:
	UFUNCTION(BlueprintCallable, Category = "CharacterBase|Function")
	class UCharacterAnimInstanceBase* GetAnimInstance() const;
	class UIKAnimInstance* GetIKAnimInstance() const;

	FORCEINLINE bool HasCrouch() const { return bCrouch; }
	FORCEINLINE bool HasSprint() const { return bSprint; }
	FORCEINLINE bool WasStanning() const { return bWasStanning; }

	float GetHealthToWidget() const;
	bool IsFullHealth() const;
	bool IsHealthHalf() const;
	bool IsHealthQuarter() const;

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
	TArray<FWeaponActionInfo> ActionInfoArray;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "CharacterBase|Combat")
	TArray<FCharacterComboInfo> ComboInfoArray;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "CharacterBase|Combat")
	TSubclassOf<class AAbstractWeapon> PrimaryWeapon;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "CharacterBase|Combat")
	TSubclassOf<class AAbstractWeapon> SecondaryWeapon;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "CharacterBase|Combat")
	class UAnimMontage* DefaultHitDamageMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "CharacterBase|Combat")
	class UParticleSystem* BloodTemplate;

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

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "CharacterBase|Debug")
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

	bool WasDebugMode() const { return bDebugTrace; }

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
	virtual void TakeDamageActionMontage(const bool InForcePlaying);
	TWeakObjectPtr<class AAbstractWeapon> CurrentWeapon;

	AAbstractWeapon* GetNotEmptyWeapon() const;
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
	virtual void FirePressed();
	virtual void FireReleassed();
	virtual void MeleeAttack();
	virtual void Reload();
	virtual void ReleaseWeapon();
	virtual void ReleaseWeaponToWorld(const FTransform& Transform, AAbstractWeapon*& Weapon);
	void CreateWeaponInstance(const TSubclassOf<class AAbstractWeapon> InWeaponTemplate, TFunction<void(AAbstractWeapon* Weapon)> Callback = nullptr);
	EWeaponItemType GetCurrentWeaponType() const;

	virtual void ReleaseAllWeaponInventory();
#pragma endregion

public:
#pragma region Utils
	UFUNCTION(BlueprintCallable, Category = "CharacterBase|Utils")
	virtual FVector BulletTraceRelativeLocation() const
	{
		if (CurrentWeapon.IsValid())
		{
			return CurrentWeapon.Get()->GetMuzzleTransform().GetLocation();
		}
		return GetActorLocation();
	}

	UFUNCTION(BlueprintCallable, Category = "CharacterBase|Utils")
	virtual FVector BulletTraceForwardLocation() const
	{
		return GetActorForwardVector();
	}

	UFUNCTION(BlueprintCallable, Category = "CharacterBase|Utils")
	FORCEINLINE EDrawDebugTrace::Type GetDrawDebugTrace() const
	{
		return bDebugTrace ? EDrawDebugTrace::Type::ForOneFrame : EDrawDebugTrace::Type::None;
	}

	FVector GetHeadSocketLocation() const;
	FVector GetChestSocketLocation() const;
#pragma endregion

	virtual void OverlapActor(AActor* InActor)
	{
		//Player Class override
		if (PickupComponent)
		{
			PickupComponent->SetPickupActor(InActor);
		}
	}

	const TArray<class AActor*>& GetIgnoreActors() { return IgnoreActors; }

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
protected:
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

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "CharacterBase|ALS")
	FRotator TargetRotation;

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterBase|ALS")
	ELSMovementMode GetALSMovementMode() const;
	virtual ELSMovementMode GetALSMovementMode_Implementation() const override
	{
		return ALSMovementMode;
	}

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterBase|ALS")
	ELSMovementAction GetALSMovementAction() const;
	virtual ELSMovementAction GetALSMovementAction_Implementation() const override
	{
		return ALSMovementAction;
	}

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterBase|ALS")
	ELSGait GetALSGait() const;
	virtual ELSGait GetALSGait_Implementation() const override
	{
		return ALSGait;
	}

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterBase|ALS")
	ELSStance GetALSStance() const;
	virtual ELSStance GetALSStance_Implementation() const override
	{
		return ALSStance;
	}

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterBase|ALS")
	ELSViewMode GetALSViewMode() const;
	virtual ELSViewMode GetALSViewMode_Implementation() const override
	{
		return ALSViewMode;
	}

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterBase|ALS")
	ELSRotationMode GetALSRotationMode() const;
	virtual ELSRotationMode GetALSRotationMode_Implementation() const override
	{
		return ALSRotationMode;
	}

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterBase|ALS")
	bool HasMovementInput() const;
	virtual bool HasMovementInput_Implementation() const override
	{
		return false;
	}

	float GetWalkingSpeed() const { return WalkingSpeed; }
	float GetRunningSpeed() const { return RunningSpeed; }
	float GetSprintingSpeed() const { return SprintingSpeed; }
	float GetCrouchingSpeed() const { return CrouchingSpeed; }
	float GetSwimmingSpeed() const { return SwimmingSpeed; }


#pragma endregion


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
	FMantleAsset DefaultLowMantleAsset;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "CharacterBase|MantleSystem")
	FMantleAsset DefaultHighMantleAsset;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "CharacterBase|MantleSystem")
	FMantleTraceSettings GroundedTraceSettings;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "CharacterBase|MantleSystem")
	FMantleTraceSettings AutomaticTraceSettings;

	UFUNCTION(BlueprintCallable, Category = "CharacterBase|MantleSystem")
	FVector GetCapsuleLocationFromBase(const FVector BaseLocation, const float ZOffset) const;

	UFUNCTION(BlueprintCallable, Category = "CharacterBase|MantleSystem")
	FVector GetCapsuleBaseLocation(const float ZOffset) const;

	UFUNCTION(BlueprintCallable, Category = "CharacterBase|MantleSystem")
	virtual FMantleAsset GetMantleAsset(const EMantleType InMantleType) const;

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

	void TraceForwardToFindWall(
		const FMantleTraceSettings InTraceSetting, 
		FVector& OutInitialTraceImpactPoint, 
		FVector& OutInitialTraceNormal, 
		bool& OutHitResult);

	void SphereTraceByMantleCheck(
		const FMantleTraceSettings TraceSetting,
		const FVector InitialTraceImpactPoint, 
		const FVector InitialTraceNormal, 
		bool &OutHitResult,
		FVector &OutDownTraceLocation,
		UPrimitiveComponent* &OutPrimitiveComponent);

	void ConvertMantleHeight(
		const FVector DownTraceLocation, 
		const FVector InitialTraceNormal, 
		bool &OutRoomCheck, 
		FTransform &OutTargetTransform, 
		float &OutMantleHeight);


	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "CharacterBase|MantleSystem")
	void BP_MantleStart(const float MantleHeight, const FLSComponentAndTransform MantleLedgeWS, const EMantleType MantleType);


#pragma endregion


};
