// Copyright 2018 wevet works All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Wevet.h"
#include "WevetTypes.h"
#include "Weapon/AbstractWeapon.h"

// Interface
#include "Interface/DamageInstigator.h"
#include "Interface/InteractionExecuter.h"
#include "Interface/GrabInstigator.h"

// Components
#include "Component/CharacterInventoryComponent.h"
#include "Component/CharacterPickupComponent.h"

// Plugins Locomotion
#include "LocomotionSystemTypes.h"
#include "CharacterBase.generated.h"

class UCharacterModel;
class UCharacterAnimInstanceBase;

UCLASS(ABSTRACT)
class WEVET_API ACharacterBase : public ACharacter, public IDamageInstigator, public IInteractionExecuter, public IGrabInstigator
{
	GENERATED_BODY()

public:
	ACharacterBase(const FObjectInitializer& ObjectInitializer);
	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void BeginDestroy() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason);
	virtual void Tick(float DeltaTime) override;
	virtual float TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

protected:
	virtual void BeginPlay() override;

public:
	virtual void Jump();
	virtual void StopJumping();
	virtual void OnSprint();
	virtual void OnCrouch();

public:
#pragma region InteractionExecuter
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterBase|InteractionExecuter")
	void OnReleaseItemExecuter();
	virtual void OnReleaseItemExecuter_Implementation() override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterBase|InteractionExecuter")
	void OnPickupItemExecuter(AActor* Actor);
	virtual void OnPickupItemExecuter_Implementation(AActor* Actor) override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterBase|InteractionExecuter")
	void ReportNoise(USoundBase* Sound, float Volume);
	virtual void ReportNoise_Implementation(USoundBase* Sound, float Volume) override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterBase|InteractionExecuter")
	void FootStep(USoundBase* Sound, float Volume);
	virtual void FootStep_Implementation(USoundBase* Sound, float Volume) override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterBase|InteractionExecuter")
	void ReportNoiseOther(AActor* Actor, USoundBase* Sound, const float Volume, const FVector Location);
	virtual void ReportNoiseOther_Implementation(AActor* Actor, USoundBase* Sound, const float Volume, const FVector Location) override;
#pragma endregion

#pragma region DamageInstigator
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterBase|IDamageInstigator")
	bool IsDeath();
	virtual bool IsDeath_Implementation() override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterBase|IDamageInstigator")
	void Die();
	virtual void Die_Implementation() override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterBase|IDamageInstigator")
	void OnTakeDamage(FName BoneName, float Damage, AActor* Actor, bool& bDied);
	virtual void OnTakeDamage_Implementation(FName BoneName, float Damage, AActor* Actor, bool& bDied) override;

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
	void ApplyDamage(UCharacterModel* DamageModel, const int InWeaponDamage, float& OutDamage);	
	virtual void ApplyDamage_Implementation(UCharacterModel* DamageModel, const int InWeaponDamage, float& OutDamage) override;
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

	UFUNCTION(BlueprintCallable)
	virtual FVector BulletTraceRelativeLocation() const;

	UFUNCTION(BlueprintCallable)
	virtual FVector BulletTraceForwardLocation() const;

	AAbstractWeapon* FindByWeapon(const EWeaponItemType WeaponItemType);

	UFUNCTION(BlueprintCallable, Category = "CharacterBase|AnimInstance")
	virtual UCharacterAnimInstanceBase* GetCharacterAnimInstance() const;

	UFUNCTION(BlueprintCallable, Category = "CharacterBase|Weapon")
	AAbstractWeapon* GetSelectedWeapon() const;

	bool HasCrouch() const;
	bool HasSprint() const;
	bool HasHanging() const;
	bool HasClimbingLedge() const;
	bool HasClimbingMoveLeft() const;
	bool HasClimbingMoveRight() const;
	bool HasEquipWeapon() const;
	virtual void ReleaseWeaponToWorld(const FTransform& Transform, AAbstractWeapon* &Weapon);

	UFUNCTION(BlueprintCallable, Category = "CharacterBase|CharacterModel")
	float GetHealthToWidget() const;

	bool IsHealthHalf() const;
	bool IsHealthQuarter() const;

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

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components, meta = (AllowPrivateAccess = "true"))
	class UPawnNoiseEmitterComponent* PawnNoiseEmitterComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components, meta = (AllowPrivateAccess = "true"))
	class UAudioComponent* AudioComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components, meta = (AllowPrivateAccess = "true"))
	class UCharacterPickupComponent* PickupComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components, meta = (AllowPrivateAccess = "true"))
	class UCharacterInventoryComponent* InventoryComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "CharacterBase|Asset|LargeWeapon")
	class UAnimMontage* EquipMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "CharacterBase|Asset|LargeWeapon")
	class UAnimMontage* UnEquipMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "CharacterBase|Asset|LargeWeapon")
	class UAnimMontage* FireMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "CharacterBase|Asset|LargeWeapon")
	class UAnimMontage* ReloadMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "CharacterBase|Asset|Damage")
	class UAnimMontage* RifleHitDamageMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "CharacterBase|Asset|Damage")
	class UAnimMontage* DefaultHitDamageMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "CharacterBase|Asset|Climbsystem")
	class UAnimMontage* ClimbLedgeMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "CharacterBase|Asset|Climbsystem")
	class UAnimMontage* ClimbJumpLeftMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "CharacterBase|Asset|Climbsystem")
	class UAnimMontage* ClimbJumpRightMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "CharacterBase|Asset|Climbsystem")
	class UAnimMontage* ClimbJumpUpMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "CharacterBase|Asset|Climbsystem")
	class UAnimMontage* ClimbCornerLeftMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "CharacterBase|Asset|Climbsystem")
	class UAnimMontage* ClimbCornerRightMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "CharacterBase|Asset")
	class USoundBase* FootStepSoundAsset;

	bool bCrouch;
	bool bWasDied;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "CharacterBase|Variable")
	bool bSprint;

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

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "CharacterBase|Variable")
	float BaseTurnRate;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "CharacterBase|Variable")
	float BaseLookUpRate;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CharacterBase|Variable")
	float MovementSpeed;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "CharacterBase|Variable")
	FName HeadSocketName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "CharacterBase|Variable")
	FName HeadBoneName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "CharacterBase|Variable")
	FName PelvisSocketName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CharacterBase|Variable")
	FName ChestSocketName;

	UPROPERTY(EditAnywhere, Instanced, Category = "CharacterBase|CharacterModel")
	UCharacterModel* CharacterModel;

	/* cacehd init speed */
	float DefaultMaxSpeed;

	/* get unequip weapon */
	AAbstractWeapon* GetUnEquipWeapon();

	/* out unequip weaponlist */
	void OutUnEquipWeaponList(TArray<AAbstractWeapon*>& OutWeaponList);

	/* pickup before had same weaponList */
	const bool SameWeapon(AAbstractWeapon* const Weapon);

	/* pickup actor */
	virtual void PickupObjects();

	/* release actor */
	virtual void ReleaseObjects();
	TWeakObjectPtr<class AAbstractWeapon> CurrentWeapon;

public:
	virtual void EquipmentActionMontage();
	virtual void UnEquipmentActionMontage();
	virtual void FireActionMontage();

	virtual void ReloadActionMontage(float &OutReloadDuration);
	virtual void TakeDamageActionMontage();

	virtual void FirePressed();
	virtual void FireReleassed();
	virtual void Reload();
	virtual void ReleaseWeapon();

protected:
	float TakeDamageInterval;
	float ComboTakeInterval;

public:
	FVector GetHeadSocketLocation() const;
	FVector GetChestSocketLocation() const;


protected:
	virtual void CreateWeaponInstance(const TSubclassOf<class AAbstractWeapon> InTemplate, bool bSetEquip = false);
};
