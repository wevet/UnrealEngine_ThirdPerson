// Copyright 2018 wevet works All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Wevet.h"
#include "WevetTypes.h"
#include "CharacterModel.h"
#include "Weapon/AbstractWeapon.h"

// Interface
#include "Interface/DamageInstigator.h"
#include "Interface/SoundInstigator.h"
#include "Interface/GrabInstigator.h"
#include "Interface/InteractionPawn.h"

// ActionInfo
#include "Structs/WeaponActionInfo.h"

// Components
#include "Component/CharacterInventoryComponent.h"
#include "Component/CharacterPickupComponent.h"

// Plugins Locomotion
#include "LocomotionSystemTypes.h"
#include "CharacterBase.generated.h"

class UCharacterAnimInstanceBase;

UCLASS(ABSTRACT)
class WEVET_API ACharacterBase : public ACharacter, public IDamageInstigator, public ISoundInstigator, public IGrabInstigator, public IInteractionPawn
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
	virtual void Jump() override;
	virtual void StopJumping() override;
	virtual void OnSprint();
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
	virtual FVector BulletTraceRelativeLocation() const;
	virtual FVector BulletTraceForwardLocation() const;
	AAbstractWeapon* FindByWeapon(const EWeaponItemType WeaponItemType);

	UFUNCTION(BlueprintCallable, Category = "CharacterBase|Function")
	virtual UCharacterAnimInstanceBase* GetCharacterAnimInstance() const;

	UFUNCTION(BlueprintCallable, Category = "CharacterBase|Function")
	AAbstractWeapon* GetSelectedWeapon() const;

	UFUNCTION(BlueprintCallable, Category = "CharacterBase|Function")
	float GetHealthToWidget() const;

	bool HasCrouch() const;
	bool HasSprint() const;
	bool HasHanging() const;
	bool HasClimbingLedge() const;
	bool HasClimbingMoveLeft() const;
	bool HasClimbingMoveRight() const;
	bool HasEquipWeapon() const;
	bool IsHealthHalf() const;
	bool IsHealthQuarter() const;
	virtual void ReleaseWeaponToWorld(const FTransform& Transform, AAbstractWeapon*& Weapon);

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

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "CharacterBase|ActionInfo")
	TArray<FWeaponActionInfo> ActionInfoArray;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "CharacterBase|Asset")
	class UAnimMontage* DefaultHitDamageMontage;

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
	virtual void EquipmentActionMontage();
	virtual void UnEquipmentActionMontage();

	TWeakObjectPtr<class AAbstractWeapon> CurrentWeapon;

public:
	virtual void FireActionMontage();

	virtual void ReloadActionMontage(float &OutReloadDuration);
	virtual void TakeDamageActionMontage();

	virtual void FirePressed();
	virtual void FireReleassed();
	virtual void Reload();
	virtual void ReleaseWeapon();
	void ReloadBulletAction();

	EWeaponItemType GetCurrentWeaponType() const;

protected:
	float TakeDamageInterval;
	float ComboTakeInterval;

public:
	FVector GetHeadSocketLocation() const;
	FVector GetChestSocketLocation() const;


protected:
	virtual void CreateWeaponInstance(const TSubclassOf<class AAbstractWeapon> InTemplate, bool bSetEquip = false);

	virtual void SetActionInfo(const EWeaponItemType InWeaponItemType, FWeaponActionInfo &OutWeaponActionInfo);
};
