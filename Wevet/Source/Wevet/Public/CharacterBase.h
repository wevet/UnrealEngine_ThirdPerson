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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ACharacterBase|Variable")
	UAnimMontage* EquipMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ACharacterBase|Variable")
	UAnimMontage* FireMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ACharacterBase|Variable")
	UAnimMontage* ReloadMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ACharacterBase|Variable")
	USoundBase* FireSound;

	float DefaultMaxSpeed;
	bool DieSuccessCalled;

public:
	virtual void Tick(float DeltaTime) override;
	virtual void Jump();
	virtual void StopJumping();
	virtual void OnSprint();
	virtual void OnCrouch();

	virtual FVector BulletTraceRelativeLocation() const { return FVector::ZeroVector; };

	virtual FVector BulletTraceForwardLocation() const { return FVector::ZeroVector; };

	UCharacterModel* GetCharacterModel() const { return this->CharacterModel; }

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

public:
	UFUNCTION(BlueprintCallable, Category = "ACharacterBase|Weapon")
	AWeaponBase* GetSelectedWeapon() const { return this->SelectedWeapon; };

	UFUNCTION(BlueprintCallable, Category = "ACharacterBase|Weapon")
	const TArray<AWeaponBase*>& GetWeaponList() { return this->WeaponList; };

	UFUNCTION(BlueprintCallable, Category = "ACharacterBase|Weapon")
	AWeaponBase* GetCategoryByWeapon(EWeaponItemType WeaponItemType);

	UFUNCTION(BlueprintCallable, Category = "ACharacterBase|Variable")
	const bool HasCrouch() { return this->IsCrouch; }

	UFUNCTION(BlueprintCallable, Category = "ACharacterBase|Variable")
	const bool HasSprint() { return this->IsSprint; }

	UFUNCTION(BlueprintCallable, Category = "ACharacterBase|Variable")
	const bool HasEquipWeapon() { return this->IsEquipWeapon; }

	UFUNCTION(BlueprintCallable, Category = "ACharacterBase|Variable")
	const float GetMaxHealth() { return this->MaxHealth; }

	UFUNCTION(BlueprintCallable, Category = "ACharacterBase|Variable")
	const float GetCurrentHealth() { return this->CurrentHealth; }

	UFUNCTION(BlueprintCallable, Category = "ACharacterBase|Variable")
	float GetHealthToWidget() const 
	{
		return this->CurrentHealth / this->MaxHealth;
	}

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ACharacterBase|Variable")
	bool IsCrouch;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ACharacterBase|Variable")
	bool IsSprint;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ACharacterBase|Variable")
	bool IsEquipWeapon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ACharacterBase|Variable")
	float BaseTurnRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ACharacterBase|Variable")
	float BaseLookUpRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ACharacterBase|Variable")
	float MovementSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ACharacterBase|Variable")
	FName HeadSocketName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ACharacterBase|Variable")
	float MaxHealth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ACharacterBase|Variable")
	float CurrentHealth;

};




