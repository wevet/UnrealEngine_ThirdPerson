// Copyright 2018 wevet works All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CharacterBase.h"
#include "WeaponBase.h"
#include "ItemBase.h"
#include "MockCharacter.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class WEVET_API AMockCharacter : public ACharacterBase
{
	GENERATED_BODY()

public:
	AMockCharacter(const FObjectInitializer& ObjectInitializer);
	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoomComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCameraComponent;

	FORCEINLINE class USpringArmComponent* GetCameraBoomComponent() const 
	{ 
		return CameraBoomComponent; 
	}

	FORCEINLINE class UCameraComponent* GetFollowCameraComponent() const 
	{
		return FollowCameraComponent; 
	}

	virtual void Die_Implementation() override;
	virtual void OnReleaseItemExecuter_Implementation() override;
	virtual void OnPickupItemExecuter_Implementation(AActor* Actor) override;
	virtual void OnTakeDamage_Implementation(FName BoneName, float Damage, AActor* Actor) override;
	virtual void Equipment_Implementation() override;
	virtual void UnEquipment_Implementation() override;
	virtual FVector BulletTraceRelativeLocation() const override;
	virtual FVector BulletTraceForwardLocation() const override;

protected:
	virtual void UpdateWeapon();
	virtual void OnCrouch() override;
	virtual void PickupObjects() override;
	virtual void ReleaseObjects() override;

	void TouchStarted(ETouchIndex::Type FingerIndex, FVector Location);
	void TouchStopped(ETouchIndex::Type FingerIndex, FVector Location);
	void TurnAtRate(float Rate);
	void LookUpAtRate(float Rate);
	void MoveForward(float Value);
	void MoveRight(float Value);
	void FirePressed();
	void FireReleassed();
	void Reload();
	void ReleaseWeapon();
	void EquipmentHandleEvent();

private:
	int32 WeaponCurrentIndex;
};

