// Copyright 2018 wevet works All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
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
#include "Interface/GrabInstigator.h"
#include "Interface/InteractionPawn.h"

// ActionInfo
#include "Structs/WeaponActionInfo.h"
#include "Structs/CharacterComboInfo.h"

// Components
#include "Component/CharacterInventoryComponent.h"
#include "Component/CharacterPickupComponent.h"
#include "Component/ComboComponent.h"

// Plugins Locomotion
#include "LocomotionSystemTypes.h"
#include "CharacterBase.generated.h"

class UCharacterAnimInstanceBase;
class UIKAnimInstance;

UCLASS(ABSTRACT)
class WEVET_API ACharacterBase : public ACharacter, public IDamageInstigator, public ISoundInstigator, public IGrabInstigator, public IInteractionPawn
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
	virtual void Jump() override;
	virtual void StopJumping() override;
	virtual void OnSprint();

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
	bool IsDeath();
	virtual bool IsDeath_Implementation() override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterBase|IDamageInstigator")
	void Die();
	virtual void Die_Implementation() override;

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
	void HitEffectReceive(const FHitResult& HitResult);
	virtual void HitEffectReceive_Implementation(const FHitResult& HitResult) override;
#pragma endregion

#pragma region IGrabInstigator
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterBase|GrabInstigator")
	void CanGrab(bool InCanGrab);
	virtual void CanGrab_Implementation(bool InCanGrab) override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterBase|GrabInstigator")
	void ClimbLedge(bool InClimbLedge);
	virtual void ClimbLedge_Implementation(bool InClimbLedge) override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterBase|GrabInstigator")
	void ReportClimbEnd();
	virtual void ReportClimbEnd_Implementation() override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterBase|GrabInstigator")
	void ClimbMove(float Value);
	virtual void ClimbMove_Implementation(float Value) override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterBase|GrabInstigator")
	void ClimbJump();
	virtual void ClimbJump_Implementation() override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterBase|GrabInstigator")
	void ReportClimbJumpEnd();
	virtual void ReportClimbJumpEnd_Implementation() override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterBase|GrabInstigator")
	void TurnConerLeftUpdate();
	virtual void TurnConerLeftUpdate_Implementation() override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterBase|GrabInstigator")
	void TurnConerRightUpdate();
	virtual void TurnConerRightUpdate_Implementation() override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterBase|GrabInstigator")
	void TurnConerResult();
	virtual void TurnConerResult_Implementation() override;
#pragma endregion

public:
	FORCEINLINE class UAudioComponent* GetAudioComponent() const { return AudioComponent; }
	FORCEINLINE class UCharacterPickupComponent* GetPickupComponent() const { return PickupComponent; }
	FORCEINLINE class UCharacterInventoryComponent* GetInventoryComponent() const { return InventoryComponent; }
	FORCEINLINE class UComboComponent* GetComboComponent() const { return ComboComponent; }

public:
	UFUNCTION(BlueprintCallable, Category = "CharacterBase|Function")
	class UCharacterAnimInstanceBase* GetAnimInstance() const;

	UFUNCTION(BlueprintCallable, Category = "CharacterBase|Function")
	class UIKAnimInstance* GetIKAnimInstance() const;

	FORCEINLINE bool HasCrouch() const { return bCrouch; }
	FORCEINLINE bool HasSprint() const { return bSprint; }
	FORCEINLINE bool HasHanging() const { return bHanging; }
	FORCEINLINE bool HasClimbingLedge() const { return bClimbingLedge; }
	FORCEINLINE bool HasClimbingMoveLeft() const { return bCanClimbMoveLeft; }
	FORCEINLINE bool HasClimbingMoveRight() const { return bCanClimbMoveRight; }

	float GetHealthToWidget() const;
	bool HasEquipWeapon() const;
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
	TSubclassOf<class AAbstractWeapon> DefaultWeapon;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "CharacterBase|Combat")
	class UAnimMontage* DefaultHitDamageMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "CharacterBase|Variable")
	class USoundBase* FootStepSoundAsset;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "CharacterBase|Combat")
	class UParticleSystem* BloodTemplate;

	UPROPERTY()
	bool bCrouch;

	UPROPERTY()
	bool bWasDied;

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
	bool bEnableRagdoll;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "CharacterBase|Debug")
	bool bDebugTrace;

#pragma region Climbsystem
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "CharacterBase|Climbsystem")
	class UAnimMontage* ClimbLedgeMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "CharacterBase|Climbsystem")
	class UAnimMontage* ClimbJumpLeftMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "CharacterBase|Climbsystem")
	class UAnimMontage* ClimbJumpRightMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "CharacterBase|Climbsystem")
	class UAnimMontage* ClimbJumpUpMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "CharacterBase|Climbsystem")
	class UAnimMontage* ClimbCornerLeftMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "CharacterBase|Climbsystem")
	class UAnimMontage* ClimbCornerRightMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "CharacterBase|Climbsystem")
	bool bHanging;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "CharacterBase|Climbsystem")
	bool bClimbingLedge;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "CharacterBase|Climbsystem")
	bool bCanClimbMoveLeft;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "CharacterBase|Climbsystem")
	bool bCanClimbMoveRight;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "CharacterBase|Climbsystem")
	bool bClimbMovingLeft;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "CharacterBase|Climbsystem")
	bool bClimbMovingRight;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "CharacterBase|Climbsystem")
	bool bCanClimbJumpLeft;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "CharacterBase|Climbsystem")
	bool bCanClimbJumpRight;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "CharacterBase|Climbsystem")
	bool bCanClimbJumpUp;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "CharacterBase|Climbsystem")
	bool bClimbJumping;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "CharacterBase|Climbsystem")
	bool bCanTurnLeft;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "CharacterBase|Climbsystem")
	bool bCanTurnRight;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "CharacterBase|Climbsystem")
	FVector HeightLocation;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "CharacterBase|Climbsystem")
	FVector WallLocation;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "CharacterBase|Climbsystem")
	FVector WallNormal;
#pragma endregion

	UPROPERTY(EditAnywhere, Instanced, Category = "CharacterBase|CharacterModel")
	UCharacterModel* CharacterModel;

	/* damage motion interval */
	float TakeDamageInterval;

protected:
#pragma region WeaponProtected
	AAbstractWeapon* FindByWeapon(const EWeaponItemType WeaponItemType) const;

	/* pickup before had same weaponList */
	const bool WasSameWeaponType(AAbstractWeapon* const Weapon);

	/* pickup actor */
	virtual void PickupObjects();

	/* release actor */
	virtual void ReleaseObjects();
	virtual void EquipmentActionMontage();
	virtual void UnEquipmentActionMontage();
	virtual void TakeDamageActionMontage();
	TWeakObjectPtr<class AAbstractWeapon> CurrentWeapon;
#pragma endregion

public:
#pragma region WeaponPublic
	FORCEINLINE class AAbstractWeapon* GetSelectedWeapon() const
	{
		if (CurrentWeapon.IsValid())
		{
			return CurrentWeapon.Get();
		}
		return nullptr;
	}

	virtual void FireActionMontage();
	virtual void ReloadActionMontage(float &OutReloadDuration);
	virtual void FirePressed();
	virtual void FireReleassed();
	virtual void Reload();
	virtual void ReleaseWeapon();
	virtual void ReleaseWeaponToWorld(const FTransform& Transform, AAbstractWeapon*& Weapon);
	virtual void CreateWeaponInstance(const TSubclassOf<class AAbstractWeapon> InWeaponTemplate, bool bSetEquip = false);
	EWeaponItemType GetCurrentWeaponType() const;
#pragma endregion

public:
#pragma region Utils
	virtual FVector BulletTraceRelativeLocation() const
	{
		return GetActorLocation();
	}

	virtual FVector BulletTraceForwardLocation() const
	{
		return GetActorForwardVector();
	}

	FVector GetHeadSocketLocation() const;
	FVector GetChestSocketLocation() const;
#pragma endregion

protected:
	virtual void SetActionInfo(const EWeaponItemType InWeaponItemType, FWeaponActionInfo &OutWeaponActionInfo);

	UFUNCTION()
	virtual void HitReceive(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	UFUNCTION(BlueprintCallable, Category = "CharacterBase|Utilities")
	void SetRagdollPhysics();

	UFUNCTION(BlueprintCallable, Category = "CharacterBase|Utilities")
	void SetForwardOrRightVector(FVector& OutForwardVector, FVector& OutRightVector);

#pragma region ALS
public:
	//

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

public:
	UFUNCTION(BlueprintCallable, Category = "CharacterBase|ALS")
	float GetWalkingSpeed() const
	{
		return WalkingSpeed;
	}

	UFUNCTION(BlueprintCallable, Category = "CharacterBase|ALS")
	float GetRunningSpeed() const
	{
		return RunningSpeed;
	}

	UFUNCTION(BlueprintCallable, Category = "CharacterBase|ALS")
	float GetSprintingSpeed() const
	{
		return SprintingSpeed;
	}

	UFUNCTION(BlueprintCallable, Category = "CharacterBase|ALS")
	float GetCrouchingSpeed() const
	{
		return CrouchingSpeed;
	}
#pragma endregion

};
