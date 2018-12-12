// Copyright 2018 wevet works All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Macros.h"
#include "Wevet.h"
#include "STypes.h"
#include "Combat.h"
#include "InteractionExecuter.h"
#include "WeaponControllerExecuter.h"
#include "CharacterBase.generated.h"

class UCharacterPickupComponent;
class UCharacterModel;

using namespace Wevet;

UCLASS(ABSTRACT)
class WEVET_API ACharacterBase : public ACharacter, public ICombat, public IInteractionExecuter
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

	virtual FVector BulletTraceRelativeLocation() const 
	{
		return FVector::ZeroVector; 
	};

	virtual FVector BulletTraceForwardLocation() const 
	{
		return FVector::ZeroVector; 
	};

#pragma region interfaces
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ACharacterBase|IInteractionExecuter")
	void OnReleaseItemExecuter();
	virtual void OnReleaseItemExecuter_Implementation() override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ACharacterBase|InteractionExecuter")
	void OnPickupItemExecuter(AActor* Actor);
	virtual void OnPickupItemExecuter_Implementation(AActor* Actor) override;

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

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ACharacterBase|ICombatExecuter")
	void NotifyEquip();
	virtual void NotifyEquip_Implementation() override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ACharacterBase|ICombatExecuter")
	UClass* GetOwnerClass() const;
	virtual UClass* GetOwnerClass_Implementation() const override;
#pragma endregion

public:
	UFUNCTION(BlueprintCallable, Category = "ACharacterBase|Weapon")
	AWeaponBase* FindByWeapon(EWeaponItemType WeaponItemType);

	UFUNCTION(BlueprintCallable, Category = "ACharacterBase|Weapon")
	AWeaponBase* GetSelectedWeapon() 
	{
		return SelectedWeapon; 
	};

	UFUNCTION(BlueprintCallable, Category = "ACharacterBase|Weapon")
	const TArray<AWeaponBase*>& GetWeaponList() 
	{
		return WeaponList; 
	};

	UFUNCTION(BlueprintCallable, Category = "ACharacterBase|Variable")
	const bool HasCrouch() 
	{
		return bCrouch; 
	}

	UFUNCTION(BlueprintCallable, Category = "ACharacterBase|Variable")
	const bool HasSprint() 
	{
		return bSprint; 
	}

	UFUNCTION(BlueprintCallable, Category = "ACharacterBase|Variable")
	virtual const bool HasEquipWeapon();

	UFUNCTION(BlueprintCallable, Category = "ACharacterBase|CharacterModel")
	float GetHealthToWidget() const;

	// @NOTE
	// GetSuper class
	//FORCEINLINE class UPawnNoiseEmitterComponent* GetPawnNoiseEmitterComponent() const
	//{
	//	return PawnNoiseEmitterComponent;
	//}

	FORCEINLINE class UAudioComponent* GetAudioComponent() const
	{
		return AudioComponent;
	}

	FORCEINLINE class UCharacterPickupComponent* GetPickupComponent() const
	{
		return PickupComponent;
	}

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components, meta = (AllowPrivateAccess = "true"))
	class UPawnNoiseEmitterComponent* PawnNoiseEmitterComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components, meta = (AllowPrivateAccess = "true"))
	class UAudioComponent* AudioComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components, meta = (AllowPrivateAccess = "true"))
	class UCharacterPickupComponent* PickupComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ACharacterBase|Weapon")
	AWeaponBase* SelectedWeapon;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ACharacterBase|Weapon")
	TArray<AWeaponBase*> WeaponList;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ACharacterBase|Asset")
	UAnimMontage* EquipMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ACharacterBase|Asset")
	UAnimMontage* FireMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ACharacterBase|Asset")
	UAnimMontage* ReloadMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ACharacterBase|Variable")
	bool bCrouch;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ACharacterBase|Variable")
	bool bSprint;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ACharacterBase|Variable")
	bool bDied;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ACharacterBase|Variable")
	float BaseTurnRate;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ACharacterBase|Variable")
	float BaseLookUpRate;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ACharacterBase|Variable")
	float MovementSpeed;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ACharacterBase|Variable")
	FName HeadSocketName;

	UPROPERTY(EditAnywhere, Instanced, Category = "ACharacterBase|CharacterModel")
	UCharacterModel* CharacterModel;

	/* cacehd init speed */
	float DefaultMaxSpeed;

	/* get unequip weapon */
	AWeaponBase* GetUnEquipWeapon();

	/* get unequip weaponlist */
	void OutUnEquipWeaponList(TArray<AWeaponBase*>& OutWeaponList);

	/* same weaponList */
	const bool SameWeapon(AWeaponBase* Weapon);

	/* pickup actor */
	virtual void PickupObjects();

	/* release actor */
	virtual void ReleaseObjects();

// blueprint native event
public:
	UFUNCTION(BlueprintImplementableEvent, Category = "ACharacterBase|NativeEvent")
	void BP_FirePressReceive();

	UFUNCTION(BlueprintImplementableEvent, Category = "ACharacterBase|NativeEvent")
	void BP_FireReleaseReceive();

	UFUNCTION(BlueprintImplementableEvent, Category = "ACharacterBase|NativeEvent")
	void BP_ReloadReceive();

// AnimMontage event
public:
	virtual void EquipmentActionMontage();
	virtual void FireActionMontage();
	virtual void ReloadActionMontage();

};
