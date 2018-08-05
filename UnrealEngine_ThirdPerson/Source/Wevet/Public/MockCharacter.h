// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CharacterBase.h"
#include "WeaponBase.h"
#include "MockCharacter.generated.h"


/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class WEVET_API AMockCharacter : public ACharacterBase
{
	GENERATED_BODY()
	
public:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoomComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCameraComponent;

	AMockCharacter(const FObjectInitializer& ObjectInitializer);
	virtual void OnConstruction(const FTransform& Transform) override;

	FORCEINLINE class USpringArmComponent* GetCameraBoomComponent() const 
	{ 
		return CameraBoomComponent; 
	}

	FORCEINLINE class UCameraComponent* GetFollowCameraComponent() const 
	{
		return FollowCameraComponent; 
	}

	virtual void Jump() override;
	virtual void StopJumping() override;
	virtual void Die_Implementation() override;
	virtual void SprintStarted() override;
	virtual void SprintStopped() override;
	virtual void UpdateSpeed() override;
	virtual void OnReleaseItemExecuter_Implementation() override;
	virtual void OnPickupItemExecuter_Implementation(AActor* Actor) override;
	virtual void OnTakeDamage_Implementation(FName BoneName, float Damage, AActor* Actor) override;
	virtual void NotifyEquip_Implementation() override;

	UFUNCTION(BlueprintCallable, Category = "AMockCharacter|ReadOnlyValiable")
	int32 GetWeaponCurrentIndex() const 
	{
		return this->WeaponCurrentIndex;
	}


protected:
	virtual void BeginPlay() override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void OnCrouch();

	UFUNCTION(BlueprintCallable, Category = "AMockCharacter|Action")
	virtual void UpdateWeapon();

	UFUNCTION(BlueprintCallable, Category = "AMockCharacter|Action")
	virtual void ReleaseItem();

	void TouchStarted(ETouchIndex::Type FingerIndex, FVector Location);
	void TouchStopped(ETouchIndex::Type FingerIndex, FVector Location);
	void TurnAtRate(float Rate);
	void LookUpAtRate(float Rate);
	void MoveForward(float Value);
	void MoveRight(float Value);
	void Equipment();

	UFUNCTION(BlueprintCallable, Category = "AMockCharacter|Action")
	AWeaponBase* ReleaseWeapon(const FTransform& Transform);

	UFUNCTION(BlueprintCallable, Category = "AMockCharacter|Action")
	AWeaponBase* GetUnEquipedWeapon();

	UPROPERTY(BlueprintReadOnly, Category = "AMockCharacter|Valiable")
	int32 WeaponCurrentIndex;

};

