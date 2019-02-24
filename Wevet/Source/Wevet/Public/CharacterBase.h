// Copyright 2018 wevet works All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Macros.h"
#include "Wevet.h"
#include "STypes.h"
#include "CombatExecuter.h"
#include "InteractionExecuter.h"
#include "GrabExecuter.h"
#include "WeaponControllerExecuter.h"
#include "CharacterBase.generated.h"

class UCharacterPickupComponent;
class UCharacterModel;
class UCharacterAnimInstanceBase;

using namespace Wevet;

UCLASS(ABSTRACT)
class WEVET_API ACharacterBase : public ACharacter, public ICombatExecuter, public IInteractionExecuter, public IGrabExecuter
{
	GENERATED_BODY()

public:
	ACharacterBase(const FObjectInitializer& ObjectInitializer);
	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void BeginDestroy() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason);
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

public:
	virtual void Jump();
	virtual void StopJumping();
	virtual void OnSprint();
	virtual void OnCrouch();

public:

#pragma region interaction
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ACharacterBase|IInteractionExecuter")
	void OnReleaseItemExecuter();
	virtual void OnReleaseItemExecuter_Implementation() override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ACharacterBase|InteractionExecuter")
	void OnPickupItemExecuter(AActor* Actor);
	virtual void OnPickupItemExecuter_Implementation(AActor* Actor) override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ACharacterBase|IInteractionExecuter")
	void ReportNoise(USoundBase* Sound, float Volume);
	virtual void ReportNoise_Implementation(USoundBase* Sound, float Volume) override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ACharacterBase|IInteractionExecuter")
	void FootStep(USoundBase* Sound, float Volume);
	virtual void FootStep_Implementation(USoundBase* Sound, float Volume) override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ACharacterBase|IInteractionExecuter")
	void ReportNoiseOther(AActor* Actor, USoundBase* Sound, const float Volume, const FVector Location);
	virtual void ReportNoiseOther_Implementation(AActor* Actor, USoundBase* Sound, const float Volume, const FVector Location) override;
#pragma endregion

#pragma region combat
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ACharacterBase|ICombatExecuter")
	bool IsDeath();
	virtual bool IsDeath_Implementation() override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ACharacterBase|ICombatExecuter")
	void Die();
	virtual void Die_Implementation() override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ACharacterBase|ICombatExecuter")
	void OnTakeDamage(FName BoneName, float Damage, AActor* Actor);
	virtual void OnTakeDamage_Implementation(FName BoneName, float Damage, AActor* Actor) override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ACharacterBase|ICombatExecuter")
	void Equipment();
	virtual void Equipment_Implementation() override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ACharacterBase|ICombatExecuter")
	void UnEquipment();
	virtual void UnEquipment_Implementation() override;
#pragma endregion

#pragma region climb
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ACharacterBase|IGrabExecuter")
	void CanGrab(bool InCanGrab);
	virtual void CanGrab_Implementation(bool InCanGrab) override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ACharacterBase|IGrabExecuter")
	void ClimbLedge(bool InClimbLedge);
	virtual void ClimbLedge_Implementation(bool InClimbLedge) override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ACharacterBase|IGrabExecuter")
	void ReportClimbEnd();
	virtual void ReportClimbEnd_Implementation() override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ACharacterBase|IGrabExecuter")
	void ClimbMove(float Value);
	virtual void ClimbMove_Implementation(float Value) override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ACharacterBase|IGrabExecuter")
	void ClimbJump();
	virtual void ClimbJump_Implementation() override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ACharacterBase|IGrabExecuter")
	void ReportClimbJumpEnd();
	virtual void ReportClimbJumpEnd_Implementation() override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ACharacterBase|IGrabExecuter")
	void TurnConerLeftUpdate();
	virtual void TurnConerLeftUpdate_Implementation() override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ACharacterBase|IGrabExecuter")
	void TurnConerRightUpdate();
	virtual void TurnConerRightUpdate_Implementation() override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ACharacterBase|IGrabExecuter")
	void TurnConerResult();
	virtual void TurnConerResult_Implementation() override;
#pragma endregion

public:
	virtual FVector BulletTraceRelativeLocation() const;
	virtual FVector BulletTraceForwardLocation() const;
	AWeaponBase* FindByWeapon(const EWeaponItemType WeaponItemType);
	UCharacterModel* GetCharacterModel() const;

	UFUNCTION(BlueprintCallable, Category = "ACharacterBase|AnimationBlueprint")
	virtual UCharacterAnimInstanceBase* GetCharacterAnimInstance() const;

	UFUNCTION(BlueprintCallable, Category = "ACharacterBase|Weapon")
	AWeaponBase* GetSelectedWeapon() const;

	const TArray<AWeaponBase*>& GetWeaponList();
	const bool HasCrouch();
	const bool HasSprint();
	const bool HasHanging();
	const bool HasClimbingLedge();
	const bool HasClimbingMoveLeft();
	const bool HasClimbingMoveRight();
	virtual const bool HasEquipWeapon();
	virtual void ReleaseWeaponToWorld(const FTransform& Transform, AWeaponBase* &Weapon);

	UFUNCTION(BlueprintCallable, Category = "ACharacterBase|CharacterModel")
	float GetHealthToWidget() const;
	bool IsHealthHalf() const;
	bool IsHealthQuarter() const;

	FORCEINLINE class UAudioComponent* GetAudioComponent() const { return AudioComponent; }
	FORCEINLINE class UCharacterPickupComponent* GetPickupComponent() const { return PickupComponent; }

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components, meta = (AllowPrivateAccess = "true"))
	class UPawnNoiseEmitterComponent* PawnNoiseEmitterComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components, meta = (AllowPrivateAccess = "true"))
	class UAudioComponent* AudioComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components, meta = (AllowPrivateAccess = "true"))
	class UCharacterPickupComponent* PickupComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ACharacterBase|Asset|LargeWeapon")
	class UAnimMontage* EquipMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ACharacterBase|Asset|LargeWeapon")
	class UAnimMontage* UnEquipMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ACharacterBase|Asset|LargeWeapon")
	class UAnimMontage* FireMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ACharacterBase|Asset|LargeWeapon")
	class UAnimMontage* ReloadMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ACharacterBase|Asset|Damage")
	class UAnimMontage* RifleHitDamageMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ACharacterBase|Asset|Damage")
	class UAnimMontage* DefaultHitDamageMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ACharacterBase|Asset|Climbsystem")
	class UAnimMontage* ClimbLedgeMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ACharacterBase|Asset|Climbsystem")
	class UAnimMontage* ClimbJumpLeftMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ACharacterBase|Asset|Climbsystem")
	class UAnimMontage* ClimbJumpRightMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ACharacterBase|Asset|Climbsystem")
	class UAnimMontage* ClimbJumpUpMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ACharacterBase|Asset|Climbsystem")
	class UAnimMontage* ClimbCornerLeftMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ACharacterBase|Asset|Climbsystem")
	class UAnimMontage* ClimbCornerRightMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ACharacterBase|Asset")
	class USoundBase* FootStepSoundAsset;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ACharacterBase|Variable")
	bool bCrouch;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ACharacterBase|Variable")
	bool bSprint;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ACharacterBase|Variable")
	bool bDied;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Climbsystem")
	bool bHanging;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Climbsystem")
	bool bClimbingLedge;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Climbsystem")
	bool bCanClimbMoveLeft;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Climbsystem")
	bool bCanClimbMoveRight;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Climbsystem")
	bool bClimbMovingLeft;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Climbsystem")
	bool bClimbMovingRight;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Climbsystem")
	bool bCanClimbJumpLeft;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Climbsystem")
	bool bCanClimbJumpRight;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Climbsystem")
	bool bCanClimbJumpUp;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Climbsystem")
	bool bClimbJumping;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Climbsystem")
	bool bCanTurnLeft;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Climbsystem")
	bool bCanTurnRight;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Climbsystem")
	FVector HeightLocation;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Climbsystem")
	FVector WallLocation;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Climbsystem")
	FVector WallNormal;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ACharacterBase|Variable")
	float BaseTurnRate;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ACharacterBase|Variable")
	float BaseLookUpRate;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ACharacterBase|Variable")
	float MovementSpeed;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ACharacterBase|Variable")
	FName HeadSocketName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ACharacterBase|Variable")
	FName PelvisSocketName;

	UPROPERTY(EditAnywhere, Instanced, Category = "ACharacterBase|CharacterModel")
	UCharacterModel* CharacterModel;

	/* cacehd init speed */
	float DefaultMaxSpeed;

	/* get unequip weapon */
	AWeaponBase* GetUnEquipWeapon();

	/* out unequip weaponlist */
	void OutUnEquipWeaponList(TArray<AWeaponBase*>& OutWeaponList);

	/* pickup before had same weaponList */
	const bool SameWeapon(AWeaponBase* const Weapon);

	/* pickup actor */
	virtual void PickupObjects();

	/* release actor */
	virtual void ReleaseObjects();
	TArray<AWeaponBase*> WeaponList;
	TWeakObjectPtr<class AWeaponBase> CurrentWeapon;

public:
	UFUNCTION(BlueprintImplementableEvent, Category = "ACharacterBase|NativeEvent")
	void BP_FirePressReceive();

	UFUNCTION(BlueprintImplementableEvent, Category = "ACharacterBase|NativeEvent")
	void BP_FireReleaseReceive();

	UFUNCTION(BlueprintImplementableEvent, Category = "ACharacterBase|NativeEvent")
	void BP_ReloadReceive();

public:
	virtual void EquipmentActionMontage();
	virtual void UnEquipmentActionMontage();
	virtual void FireActionMontage();
	virtual void ReloadActionMontage();
	virtual void TakeDamageActionMontage();

protected:
	float TakeDamageInterval;
	float ComboTakeInterval;
};
