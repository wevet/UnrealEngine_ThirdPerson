// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Macros.h"
#include "WeaponBase.h"
#include "Combat.h"
#include "InteractionExecuter.h"
#include "CharacterModel.h"
#include "CharacterBase.generated.h"


UCLASS(ABSTRACT)
class WEVET_API ACharacterBase : public ACharacter, public ICombat, public IInteractionExecuter
{
	GENERATED_BODY()

public:
	ACharacterBase(const FObjectInitializer& ObjectInitializer);
	virtual void OnConstruction(const FTransform& Transform) override;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ACharacterBase|Weapon")
	AWeaponBase* SelectedWeapon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ACharacterBase|Weapon")
	TArray<AWeaponBase*> WeaponList;

	UPROPERTY(EditAnywhere, Instanced, Category = "ACharacterBase|Model")
	UCharacterModel* CharacterModel;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ACharacterBase|Valiable")
	UAnimMontage* EquipMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ACharacterBase|Valiable")
	UAnimMontage* FireMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ACharacterBase|Valiable")
	UAnimMontage* ReloadMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ACharacterBase|Valiable")
	USoundBase* FireSound;

	float DefaultMaxSpeed;
	bool DieSuccessCalled;

public:
	virtual void Tick(float DeltaTime) override;
	virtual void Jump();
	virtual void StopJumping();
	virtual void SprintStarted();
	virtual void SprintStopped();
	virtual void UpdateSpeed();


	UFUNCTION(BlueprintCallable, Category = "ACharacterBase|Action")
	virtual void AttachWeapon();

	UCharacterModel* GetCharacterModel() const 
	{
		return this->CharacterModel; 
	}

#pragma region InteractionExecuter
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ACharacterBase|IInteractionExecuter")
	void OnReleaseItemExecuter();
	virtual void OnReleaseItemExecuter_Implementation() override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ACharacterBase|InteractionExecuter")
	void OnPickupItemExecuter(AActor* Actor);
	virtual void OnPickupItemExecuter_Implementation(AActor* Actor) override;
#pragma endregion


#pragma region CombatExecuter
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
#pragma endregion


	UFUNCTION(BlueprintCallable, Category = "ACharacterBase|ICombatExecuter")
	virtual void UpdateCrouch(bool Crouch);

	UFUNCTION(BlueprintCallable, Category = "ACharacterBase|Weapon")
	AWeaponBase* GetSelectedWeapon() const;

	UFUNCTION(BlueprintCallable, Category = "ACharacterBase|Weapon")
	TArray<AWeaponBase*> GetWeaponList() const;

	UFUNCTION(BlueprintCallable, Category = "ACharacterBase|Weapon")
	AWeaponBase* GetCategoryByWeapon(EWeaponItemType WeaponItemType);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ACharacterBase|ICombatExecuter")
	bool IsCrouch;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ACharacterBase|ICombatExecuter")
	bool IsEquipWeapon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ACharacterBase|Valiable")
	float BaseTurnRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ACharacterBase|Valiable")
	float BaseLookUpRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ACharacterBase|Valiable")
	float MovementSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ACharacterBase|Valiable")
	FName HeadSocketName;

	// @TODO
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ACharacterBase|Model")
	float MaxHealth;

	// @TODO
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ACharacterBase|Model")
	float CurrentHealth;


	bool Sprint;
};




