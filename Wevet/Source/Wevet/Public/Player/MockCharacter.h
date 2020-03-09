// Copyright 2018 wevet works All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Character/CharacterBase.h"
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
	FORCEINLINE class USpringArmComponent* GetCameraBoomComponent() const 
	{ 
		return CameraBoomComponent; 
	}

	FORCEINLINE class UCameraComponent* GetTPSCameraComponent() const 
	{
		return TPSCameraComponent; 
	}

	FORCEINLINE class UCameraComponent* GetFPSCameraComponent() const
	{
		return FPSCameraComponent;
	}

	virtual void Die_Implementation() override;
	virtual void OnReleaseItemExecuter_Implementation() override;
	virtual void OnPickupItemExecuter_Implementation(AActor* Actor) override;
	virtual void OnTakeDamage_Implementation(FName BoneName, float Damage, AActor* Actor, bool& bDied) override;
	virtual void Equipment_Implementation() override;
	virtual void UnEquipment_Implementation() override;
	virtual void ClimbLedge_Implementation(bool InClimbLedge) override;
	virtual void ClimbJump_Implementation() override;
	virtual void ReportClimbJumpEnd_Implementation() override;
	virtual void TurnConerResult_Implementation() override;

	virtual FVector BulletTraceRelativeLocation() const override;
	virtual FVector BulletTraceForwardLocation() const override;
	virtual void EquipmentActionMontage() override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoomComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* TPSCameraComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FPSCameraComponent;

protected:
	virtual void UpdateWeapon();
	virtual void OnCrouch() override;
	virtual void PickupObjects() override;
	virtual void ReleaseObjects() override;
	virtual void Jump() override;
	virtual void StopJumping() override;
	virtual void FirePressed() override;
	virtual void FireReleassed() override;
	virtual void Reload() override;
	virtual void ReleaseWeapon() override;

	void TurnAtRate(float Rate);
	void LookUpAtRate(float Rate);
	void MoveForward(float Value);
	void MoveRight(float Value);
	void ToggleEquip();

private:
	int32 WeaponCurrentIndex;
};

