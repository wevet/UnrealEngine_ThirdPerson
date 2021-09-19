// Copyright 2018 wevet works All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Curves/CurveFloat.h"
#include "Curves/CurveVector.h"
#include "Wevet.h"
#include "WevetTypes.h"
#include "CharacterModel.h"

// AIModule
#include "Perception/AISightTargetInterface.h"
#include "GenericTeamAgentInterface.h"

#include "Item/AbstractWeapon.h"
#include "Item/AbstractItem.h"

// Interface
#include "Interface/CombatInstigator.h"
#include "Interface/SoundInstigator.h"
#include "Interface/InteractionPawn.h"
#include "Interface/BrainInstigator.h"

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
#include "LocomotionSystemPawn.h"
#include "CharacterBase.generated.h"

using WeaponFunc = TFunction<void(AAbstractWeapon* Weapon)>;
class UCharacterAnimInstanceBase;
class UIKAnimInstance;

UCLASS(ABSTRACT)
class WEVET_API ACharacterBase : public ACharacter, public IBrainInstigator, public ICombatInstigator, public ISoundInstigator, public IInteractionPawn, public ILocomotionSystemPawn, public IAISightTargetInterface, public IGenericTeamAgentInterface
{
	GENERATED_BODY()

public:
	ACharacterBase(const FObjectInitializer& ObjectInitializer);
	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void PostInitializeComponents() override;
	virtual void BeginDestroy() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void Tick(float DeltaTime) override;
	virtual float TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

protected:
	virtual void BeginPlay() override;
	virtual void OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode = 0) override;
	virtual void OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;
	virtual void OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;

protected:
	UPROPERTY(BlueprintAssignable)
	FCombatDelegate DeathDelegate;

	UPROPERTY(BlueprintAssignable)
	FCombatDelegate AliveDelegate;

	UPROPERTY(BlueprintAssignable)
	FCombatOneDelegate KillDelegate;

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
	virtual bool CanBeSeenFrom(const FVector& ObserverLocation, FVector& OutSeenLocation, int32& NumberOfLoSChecksPerformed, float& OutSightStrength, const AActor* IgnoreActor = nullptr, const bool* bWasVisible = nullptr, int32* UserData = nullptr) const override;


protected:
	virtual void TurnAtRate(float Rate);
	virtual void LookUpAtRate(float Rate);
	virtual void MoveForward(float Value);
	virtual void MoveRight(float Value);

	UFUNCTION(BlueprintCallable, Category = "CharacterBase|Function")
	virtual void OnCrouch();

	UFUNCTION(BlueprintCallable, Category = "CharacterBase|Function")
	virtual void CrouchUpdate(const bool InCrouch);

	virtual void PickupObjects();
	virtual void ReleaseObjects();
	virtual void OnWalkAction();

public:
	virtual void MeleeAttack(const bool InEnable);


public:
#pragma region Interaction
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterBase|InteractionPawn")
	void Pickup(const EItemType InItemType, AActor* Actor);
	virtual void Pickup_Implementation(const EItemType InItemType, AActor* Actor) override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterBase|InteractionPawn")
	bool CanPickup() const;
	virtual bool CanPickup_Implementation() const override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterBase|InteractionPawn")
	void OverlapActor(AActor* Actor);
	virtual void OverlapActor_Implementation(AActor* Actor) override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterBase|InteractionPawn")
	void Release();
	virtual void Release_Implementation() override;
#pragma endregion


#pragma region Sound
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


#pragma region Combat
	virtual FCombatDelegate* GetDeathDelegate() override;
	virtual FCombatDelegate* GetAliveDelegate() override;
	virtual FCombatOneDelegate* GetKillDelegate() override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterBase|CombatInstigator")
	float GetMeleeDistance() const;
	virtual float GetMeleeDistance_Implementation() const override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterBase|CombatInstigator")
	AActor* GetTarget() const;
	virtual AActor* GetTarget_Implementation() const override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterBase|CombatInstigator")
	void SetActionState(const EAIActionState InAIActionState);
	virtual void SetActionState_Implementation(const EAIActionState InAIActionState) override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterBase|CombatInstigator")
	void OnActionStateChange();
	virtual void OnActionStateChange_Implementation() override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterBase|CombatInstigator")
	EAIActionState GetActionState() const;
	virtual EAIActionState GetActionState_Implementation() const override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterBase|CombatInstigator")
	bool CanStrike() const;
	virtual bool CanStrike_Implementation() const override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterBase|CombatInstigator")
	void InfrictionDamage(AActor* InfrictionActor, const bool bInfrictionDie);
	virtual void InfrictionDamage_Implementation(AActor* InfrictionActor, const bool bInfrictionDie) override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterBase|CombatInstigator")
	bool IsDeath() const;
	virtual bool IsDeath_Implementation() const override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterBase|CombatInstigator")
	bool IsStan() const;
	virtual bool IsStan_Implementation() const override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterBase|CombatInstigator")
	void Die();
	virtual void Die_Implementation() override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterBase|CombatInstigator")
	void Alive();
	virtual void Alive_Implementation() override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterBase|CombatInstigator")
	void Equipment();
	virtual void Equipment_Implementation() override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterBase|CombatInstigator")
	void UnEquipment();
	virtual void UnEquipment_Implementation() override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterBase|CombatInstigator")
	UCharacterModel* GetPropertyModel() const;
	virtual UCharacterModel* GetPropertyModel_Implementation() const override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterBase|CombatInstigator")
	float MakeDamage(UCharacterModel* DamageModel, const int InWeaponDamage) const;
	virtual float MakeDamage_Implementation(UCharacterModel* DamageModel, const int InWeaponDamage) const override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterBase|CombatInstigator")
	bool CanKillDealDamage(const FName BoneName) const;
	virtual bool CanKillDealDamage_Implementation(const FName BoneName) const override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterBase|CombatInstigator")
	void HitEffectReceive(const FHitResult& HitResult, const EGiveDamageType InGiveDamageType);
	virtual void HitEffectReceive_Implementation(const FHitResult& HitResult, const EGiveDamageType InGiveDamageType) override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterBase|CombatInstigator")
	void DoFirePressed();
	virtual void DoFirePressed_Implementation() override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterBase|CombatInstigator")
	void DoFireReleassed();
	virtual void DoFireReleassed_Implementation() override;	

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterBase|CombatInstigator")
	void DoReload();
	virtual void DoReload_Implementation() override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterBase|CombatInstigator")
	AAbstractWeapon* GetCurrentWeapon() const;
	virtual AAbstractWeapon* GetCurrentWeapon_Implementation() const override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterBase|CombatInstigator")
	FVector BulletTraceRelativeLocation() const;
	virtual FVector BulletTraceRelativeLocation_Implementation() const override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterBase|CombatInstigator")
	FVector BulletTraceForwardLocation() const;
	virtual FVector BulletTraceForwardLocation_Implementation() const override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterBase|CombatInstigator")
	void FireActionMontage();
	virtual void FireActionMontage_Implementation() override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterBase|CombatInstigator")
	void ReloadActionMontage(float& OutReloadDuration);
	virtual void ReloadActionMontage_Implementation(float& OutReloadDuration) override;
#pragma endregion


#pragma region Brain
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterBase|BrainInstigator")
	class UBehaviorTree* GetBehaviorTree() const;
	virtual class UBehaviorTree* GetBehaviorTree_Implementation() const override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterBase|BrainInstigator")
	void DoSightReceive(AActor* Actor, const FAIStimulus InStimulus, const bool InWasKilledCrew);
	virtual void DoSightReceive_Implementation(AActor* Actor, const FAIStimulus InStimulus, const bool InWasKilledCrew) override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterBase|BrainInstigator")
	void DoHearReceive(AActor* Actor, const FAIStimulus InStimulus, const bool InWasKilledCrew);
	virtual void DoHearReceive_Implementation(AActor* Actor, const FAIStimulus InStimulus, const bool InWasKilledCrew) override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterBase|BrainInstigator")
	void DoPredictionReceive(AActor* Actor, const FAIStimulus InStimulus);
	virtual void DoPredictionReceive_Implementation(AActor* Actor, const FAIStimulus InStimulus) override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterBase|BrainInstigator")
	void DoDamageReceive(AActor* Actor, const FAIStimulus InStimulus);
	virtual void DoDamageReceive_Implementation(AActor* Actor, const FAIStimulus InStimulus) override;
#pragma endregion


public:
	FORCEINLINE class UAudioComponent* GetAudioComponent() const { return AudioComponent; }
	FORCEINLINE class UCharacterPickupComponent* GetPickupComponent() const { return PickupComponent; }
	FORCEINLINE class UCharacterInventoryComponent* GetInventoryComponent() const { return InventoryComponent; }
	FORCEINLINE class UComboComponent* GetComboComponent() const { return ComboComponent; }
	FORCEINLINE class UPawnNoiseEmitterComponent* GetPawnNoiseEmitterComponent() const { return PawnNoiseEmitterComponent; }

public:
	float GetHealthToWidget() const { return CharacterModel->GetHealthToWidget(); }
	bool IsFullHealth() const { return CharacterModel->IsFullHealth(); }
	bool IsHealthHalf() const { return CharacterModel->IsHealthHalf(); }
	bool IsHealthQuarter() const { return CharacterModel->IsHealthQuarter(); }
	bool WasMeleeAttackPlaying() const { return MeleeAttackTimeOut >= ZERO_VALUE; }
	bool WasTakeDamagePlaying() const { return TakeDamageTimeOut >= ZERO_VALUE; }
	bool WasEquipWeaponPlaying() const { return EquipWeaponTimeOut >= ZERO_VALUE; }

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
#pragma region Uproperty
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "CharacterBase|Combat")
	TSubclassOf<class AAbstractWeapon> PrimaryWeapon;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "CharacterBase|Combat")
	TSubclassOf<class AAbstractWeapon> SecondaryWeapon;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "CharacterBase|Combat")
	TSubclassOf<class AAbstractWeapon> NakedWeapon;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "CharacterBase|Combat")
	class UBehaviorTree* BehaviorTree;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "CharacterBase|Combat")
	class UParticleSystem* BloodTemplate;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "CharacterBase|Variable")
	struct FGenericTeamId TeamId;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "CharacterBase|Variable")
	class USoundBase* FootStepSoundAsset;

	UPROPERTY()
	bool bWasDied;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "CharacterBase|Variable")
	float BaseTurnRate;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "CharacterBase|Variable")
	float BaseLookUpRate;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "CharacterBase|Variable")
	float ForwardAxisValue;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "CharacterBase|Variable")
	float RightAxisValue;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "CharacterBase|Variable")
	float RecoverTimer;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "CharacterBase|Variable")
	int32 RecoverHealthValue;

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
	bool bWasSprint;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "CharacterBase|Variable")
	bool bEnableRagdoll;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "CharacterBase|Variable")
	bool bEnableRecover;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "CharacterBase|Variable")
	bool bDebugTrace;
#pragma endregion


protected:
	UPROPERTY(EditAnywhere, Instanced, Category = "CharacterBase|CharacterModel")
	UCharacterModel* CharacterModel;

	UPROPERTY()
	TArray<class UMeshComponent*> MeshComponents;

	UPROPERTY()
	ACharacterBase* TargetCharacter;

	/* damage motion timeout */
	float TakeDamageTimeOut;

	/* melee attack motion timeout */
	float MeleeAttackTimeOut;

	/* for ai equip weapon timeout */
	float EquipWeaponTimeOut;

	/* melee attack received pawn timeout */
	float StanTimeOut;

	float RecoverInterval;
	FWeaponActionInfo* ActionInfoPtr;
	FTimerHandle MeleeAttackHundle;
	FTimerHandle StanHundle;
	EGiveDamageType GiveDamageType;
	EAIActionState ActionState;
	TArray<class AActor*> IgnoreActors;


protected:
	void UpdateCombatTimer(const float InDeltaTime);
	void UpdateRecoverTimer(const float InDeltaTime);


#pragma region Weapon
public:
	UFUNCTION(BlueprintCallable, Category = "CharacterBase|Function")
	bool HasEquipWeapon() const;

	UFUNCTION(BlueprintCallable, Category = "CharacterBase|Function")
	bool HasEmptyWeapon() const;

	UFUNCTION(BlueprintCallable, Category = "CharacterBase|Function")
	EWeaponItemType GetCurrentWeaponType() const;

	UFUNCTION(BlueprintCallable, Category = "CharacterBase|Function")
	AAbstractWeapon* SwitchWeaponAction() const;

protected:
	AAbstractWeapon* FindByWeapon(const EWeaponItemType WeaponItemType) const;
	const bool WasSameWeaponType(AAbstractWeapon* const Weapon);
	virtual void CreateWeaponInstance(const TSubclassOf<class AAbstractWeapon> InWeaponTemplate, WeaponFunc Callback = nullptr);
	virtual void ReleaseAllWeaponInventory();
	virtual void ReleaseAllItemInventory();
	void ReleaseWeaponToWorld(const FTransform& Transform, AAbstractWeapon*& Weapon);
	void ReleaseItemToWorld(const FTransform& Transform, AAbstractItem*& Item);

	UFUNCTION()
	void WeaponFireCallBack(const bool InFiredAction);

protected:
	TWeakObjectPtr<class AAbstractWeapon> CurrentWeapon;
#pragma endregion


protected:
	virtual void EquipmentActionMontage();
	virtual void UnEquipmentActionMontage();
	virtual void TakeDamageMontage(const bool InForcePlaying);


public:
#pragma region Utils
	UFUNCTION(BlueprintCallable, Category = "CharacterBase|Function")
	class UCharacterAnimInstanceBase* GetAnimInstance() const;
	class UIKAnimInstance* GetIKAnimInstance() const;

	FVector GetHeadSocketLocation() const;
	FVector GetChestSocketLocation() const;
	const TArray<class AActor*>& GetIgnoreActors();

	UFUNCTION(BlueprintCallable, Category = "CharacterBase|Utils")
	EDrawDebugTrace::Type GetDrawDebugTrace() const;

	UFUNCTION(BlueprintCallable, Category = "CharacterBase|Utils")
	FTransform GetChestTransform() const;

	void SetActionInfo(const EWeaponItemType InWeaponItemType);

	void SetEnableRecover(const bool InEnableRecover);
#pragma endregion

protected:
	UFUNCTION()
	virtual void HitReceive(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
	void KillRagdollPhysics();
	void RemoveBindAll();

public:
	UFUNCTION(BlueprintCallable, Category = "CharacterBase|Ragdoll")
	virtual void StartRagdollAction();

	UFUNCTION(BlueprintCallable, Category = "CharacterBase|Ragdoll")
	virtual void RagdollToWakeUpAction();


protected:
#pragma region ALS_Property
	UPROPERTY(Replicated, BlueprintReadWrite)
	FVector MovementInput;

	UPROPERTY(Replicated, BlueprintReadWrite)
	FVector RagdollLocation;

	UPROPERTY(Replicated, BlueprintReadWrite)
	FRotator CharacterRotation;

	UPROPERTY(Replicated, BlueprintReadWrite)
	FRotator LookingRotation;

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

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "CharacterBase|ALS")
	FMantleParams MantleParams;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "CharacterBase|ALS")
	FLSComponentAndTransform MantleLedgeLS;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "CharacterBase|ALS")
	FTransform MantleTarget;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "CharacterBase|ALS")
	FTransform MantleActualStartOffset;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "CharacterBase|ALS")
	FTransform MantleAnimatedStartOffset;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "CharacterBase|ALS")
	FMantleTraceSettings FallingTraceSettings;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "CharacterBase|ALS")
	FMantleTraceSettings GroundedTraceSettings;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "CharacterBase|ALS")
	FMantleTraceSettings AutomaticTraceSettings;
#pragma endregion


public:
#pragma region ALS_Getter
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterBase|ALS_Pawn")
	ELSMovementMode GetALSMovementMode() const;
	virtual ELSMovementMode GetALSMovementMode_Implementation() const override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterBase|ALS_Pawn")
	ELSMovementAction GetALSMovementAction() const;
	virtual ELSMovementAction GetALSMovementAction_Implementation() const override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterBase|ALS_Pawn")
	ELSGait GetALSGait() const;
	virtual ELSGait GetALSGait_Implementation() const override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterBase|ALS_Pawn")
	ELSStance GetALSStance() const;
	virtual ELSStance GetALSStance_Implementation() const override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterBase|ALS_Pawn")
	ELSViewMode GetALSViewMode() const;
	virtual ELSViewMode GetALSViewMode_Implementation() const override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterBase|ALS_Pawn")
	ELSRotationMode GetALSRotationMode() const;
	virtual ELSRotationMode GetALSRotationMode_Implementation() const override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterBase|ALS_Pawn")
	FCameraFOVParam GetCameraFOVParam() const;
	FCameraFOVParam GetCameraFOVParam_Implementation() const override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterBase|ALS_Pawn")
	FCameraTraceParam GetCameraTraceParam() const;
	FCameraTraceParam GetCameraTraceParam_Implementation() const override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterBase|ALS_Pawn")
	FTransform GetPivotTarget() const;
	virtual FTransform GetPivotTarget_Implementation() const override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterBase|ALS_Pawn")
	FVector GetCameraTarget() const;
	virtual FVector GetCameraTarget_Implementation() const override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterBase|ALS_Pawn")
	bool HasMovementInput() const;
	virtual bool HasMovementInput_Implementation() const override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterBase|ALS_Pawn")
	bool HasMoving() const;
	virtual bool HasMoving_Implementation() const override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterBase|ALS_Pawn")
	bool HasAiming() const;
	virtual bool HasAiming_Implementation() const override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterBase|ALS_Pawn")
	bool HasDebugTrace() const;
	virtual bool HasDebugTrace_Implementation() const override;

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
#pragma endregion


#pragma region ALS_Setter
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterBase|ALS_Pawn")
	void SetALSCharacterRotation(const FRotator AddAmount);
	virtual void SetALSCharacterRotation_Implementation(const FRotator AddAmount) override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterBase|ALS_Pawn")
	void SetALSCameraShake(TSubclassOf<class UCameraShakeBase> InShakeClass, const float InScale);
	virtual void SetALSCameraShake_Implementation(TSubclassOf<class UCameraShakeBase> InShakeClass, const float InScale) override;

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
#pragma endregion


#pragma region ALS_NotUseAPI
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterBase|ALS_Pawn")
	void SetALSIdleState(const ELSIdleEntryState InLSIdleEntryState);
	virtual void SetALSIdleState_Implementation(const ELSIdleEntryState InLSIdleEntryState) override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterBase|ALS_Pawn")
	void SetGetup(const bool InFaceDown);
	virtual void SetGetup_Implementation(const bool InFaceDown) override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterBase|ALS_Pawn")
	void SetRF(const bool InRF);
	virtual void SetRF_Implementation(const bool InRF) override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterBase|ALS_Pawn")
	void PoseSnapShot(const FName InPoseName);
	virtual void PoseSnapShot_Implementation(const FName InPoseName) override;
#pragma endregion


#pragma region ALSFunction
public:
	virtual FVector ChooseVelocity() const;
	float ChooseMaxWalkSpeed() const;

	FORCEINLINE float ChooseMaxAcceleration() const
	{
		return (ALSGait == ELSGait::Walking || ALSMovementMode == ELSMovementMode::Swimming) ? WalkingAcceleration : RunningAcceleration;
	}

	FORCEINLINE float ChooseBrakingDeceleration() const
	{
		return (ALSGait == ELSGait::Walking || ALSMovementMode == ELSMovementMode::Swimming) ? WalkingDeceleration : RunningDeceleration;
	}

	FORCEINLINE float ChooseGroundFriction() const
	{
		return (ALSGait == ELSGait::Walking || ALSMovementMode == ELSMovementMode::Swimming) ? WalkingGroundFriction : RunningGroundFriction;
	}

	FORCEINLINE float GetAimYawDelta() const { return AimYawDelta; }
	FORCEINLINE float GetAimYawRate() const { return AimYawRate; }
	FORCEINLINE float GetVelocityDifference() const { return VelocityDifference; }
	FORCEINLINE float GetRotationDifference() const { return RotationDifference; }
	FORCEINLINE float GetDirection() const { return Direction; }
	FORCEINLINE FRotator GetLastVelocityRotation() const { return LastVelocityRotation; }
	FORCEINLINE FRotator GetLastMovementInputRotation() const { return LastMovementInputRotation; }
	FORCEINLINE FRotator GetCharacterRotation() const { return CharacterRotation; }
	FORCEINLINE FRotator GetLookingRotation() const { return LookingRotation; }

protected:
	UFUNCTION(BlueprintCallable, Category = "CharacterBase|ALS")
	void ManageCharacterRotation();

	UFUNCTION(BlueprintCallable, Category = "CharacterBase|ALS")
	virtual void DoWhileALSMovementMode();

	UFUNCTION(BlueprintCallable, Category = "CharacterBase|ALS")
	bool CanSprint() const;

	UFUNCTION(BlueprintCallable, Category = "CharacterBase|ALS")
	void AddCharacterRotation(const FRotator AddAmount);

	void DoWhileGrounded();
	void DoWhileMantling();
	void DoWhileRagdolling();

	void UpdateRagdollTransform(FRotator& OutActorRotation, FVector& OutActorLocation);
	void CalcurateRagdollParams(const FVector InRagdollVelocity, const FVector InRagdollLocation, const FRotator InActorRotation, const FVector InActorLocation);
	void DoCharacterFalling();
	void DoCharacterGrounded();
	void UpdateCharacterMovementSettings();
	void MovementInputControl(const bool bForwardAxis);
	void GroundMovementInput(const bool bForwardAxis);
	void RagdollMovementInput();

	void CalculateActorTransformRagdoll(const FRotator InRagdollRotation, const FVector InRagdollLocation, FRotator& OutActorRotation, FVector& OutActorLocation);
	void CalculateEssentialVariables();
	void SetForwardOrRightVector(FVector& OutForwardVector, FVector& OutRightVector);
	void ConvertALSMovementMode();
	const float CalculateRotationRate(const float SlowSpeed, const float SlowSpeedRate, const float FastSpeed, const float FastSpeedRate);
	const FRotator LookingDirectionWithOffset(const float OffsetInterpSpeed, const float NEAngle, const float NWAngle, const float SEAngle, const float SWAngle, const float Buffer);
	ELSMovementMode GetPawnMovementModeChanged(const EMovementMode PrevMovementMode, const uint8 PrevCustomMode) const;

	void ApplyCharacterRotation(const FRotator InTargetRotation, const bool bInterpRotation, const float InterpSpeed);
	void LimitRotation(const float AimYawLimit, const float InterpSpeed);
	bool CardinalDirectionAngles(const float Value, const float Min, const float Max, const float Buffer, const ELSCardinalDirection InCardinalDirection) const;
	void CustomAcceleration();
#pragma endregion


#pragma region MantleSystem
protected:
	UFUNCTION(BlueprintCallable, Category = "CharacterBase|MantleSystem")
	void MantleStart(const float InMantleHeight, const FLSComponentAndTransform MantleLedgeWorldSpace, const EMantleType InMantleType, UTimelineComponent* const InMantleTimeline);

	UFUNCTION(BlueprintCallable, Category = "CharacterBase|MantleSystem")
	void MantleUpdate(const float BlendIn, const float InPlaybackPosition);

	UFUNCTION(BlueprintCallable, Category = "CharacterBase|MantleSystem")
	void MantleEnd();

	UFUNCTION(BlueprintCallable, Category = "CharacterBase|MantleSystem")
	void SetActorLocationAndRotation(const FVector NewLocation, const FRotator NewRotation, const bool bWasSweep, const bool bWasTeleport);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "CharacterBase|MantleSystem")
	void BP_MantleStart(const float MantleHeight, const FLSComponentAndTransform MantleLedgeWS, const EMantleType MantleType);

	const bool MantleCheck(const FMantleTraceSettings InTraceSetting);

	// Mantle Utils
	FVector GetCapsuleBaseLocation(const float ZOffset) const;
	FVector GetCapsuleLocationFromBase(const FVector BaseLocation, const float ZOffset) const;
	const FVector GetPlayerMovementInput();
	bool CapsuleHasRoomCheck(const FVector TargetLocation, const float HeightOffset, const float RadiusOffset) const;

	// MantleCheck Details
	void TraceForwardToFindWall(const FMantleTraceSettings InTraceSetting, FVector& OutInitialTraceImpactPoint, FVector& OutInitialTraceNormal, bool& OutHitResult);
	void SphereTraceByMantleCheck(const FMantleTraceSettings TraceSetting, const FVector InitialTraceImpactPoint, const FVector InitialTraceNormal, bool &OutHitResult, FVector &OutDownTraceLocation, UPrimitiveComponent* &OutPrimitiveComponent);
	void ConvertMantleHeight(const FVector DownTraceLocation, const FVector InitialTraceNormal, bool &OutRoomCheck, FTransform &OutTargetTransform, float &OutMantleHeight);
	EMantleType GetMantleType(const float InMantleHeight) const;

	// MantleUpdate Details
	void SetMantleTarget();
	void SetMantleUpdateAlpha(const float PlaybackPoint, float& OutPositionAlpha, float& OutXY, float& OutZ);
	FTransform MakeXYCollectionAlphaTransform(const float InXYCollectionAlpha) const;
	FTransform MakeZCollectionAlphaTransform(const float InZCollectionAlpha) const;
	FTransform MakeMantleTransform(const float Position, const float XY, const float Z) const;
	FTransform MakeMantleLerpedTarget(const float Blend, const float Position, const float XY, const float Z) const;

	// MantleStart Details
	void SetMantleParams(FMantleAsset InMantleAsset, const float InMantleHeight);
	void SetMantleLedgeLocalSpace(const FLSComponentAndTransform MantleLedgeWorldSpace);
	void CalculateMantleTarget(const FLSComponentAndTransform MantleLedgeWorldSpace);
	void CalculateMantleAnimatedStartOffset();
	void PlayFromStartMantle(UTimelineComponent* const InMantleTimeline);
#pragma endregion


};

