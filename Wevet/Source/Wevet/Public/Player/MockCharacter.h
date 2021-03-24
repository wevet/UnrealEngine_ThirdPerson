// Copyright 2018 wevet works All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Character/CharacterBase.h"
#include "Tools/BackPack.h"
#include "Components/PostProcessComponent.h"
#include "MockCharacter.generated.h"


class AMockPlayerController;
class UPlayerAnimInstance;

UCLASS(Blueprintable, BlueprintType)
class WEVET_API AMockCharacter : public ACharacterBase
{
	GENERATED_BODY()

public:
	AMockCharacter(const FObjectInitializer& ObjectInitializer);
	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;


#pragma region Interface
public:
	// DamageInstigator
	virtual void Die_Implementation() override;
	virtual void Alive_Implementation() override;
	virtual void Equipment_Implementation() override;
	virtual void UnEquipment_Implementation() override;
	
	// InteractionPawn
	virtual bool CanPickup_Implementation() const override;
	virtual void Release_Implementation() override;
	virtual void OverlapActor_Implementation(AActor* Actor) override;

	// ALS
	virtual void SetALSCameraShake_Implementation(TSubclassOf<class UMatineeCameraShake> InShakeClass, const float InScale) override;
#pragma endregion


public:
	virtual FVector BulletTraceRelativeLocation() const override;
	virtual FVector BulletTraceForwardLocation() const override;
	virtual void EquipmentActionMontage() override;
	virtual void CreateWeaponInstance(const TSubclassOf<class AAbstractWeapon> InWeaponTemplate, WeaponFunc Callback = nullptr) override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components, meta = (AllowPrivateAccess = "true"))
	class UPostProcessComponent* OutlinePostProcessComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components, meta = (AllowPrivateAccess = "true"))
	class UPostProcessComponent* DeathPostProcessComponent;

protected:
	UPROPERTY()
	class AMockPlayerController* PlayerController;

	UPROPERTY()
	class UPlayerAnimInstance* PlayerAnimInstance;

	UPROPERTY()
	class ABackPack* BackPack;

	UPROPERTY()
	TArray<class UMeshComponent*> MeshArray;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Player|Variable")
	TSubclassOf<class ABackPack> BackPackTemplate;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Player|Variable")
	FName CameraTraceLSocket;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Player|Variable")
	FName CameraTraceRSocket;

	int32 WeaponCurrentIndex;

public:
	UFUNCTION(BlueprintCallable, Category = "Player|Function")
	void SetOwnerNoSeeMesh(const bool NewOwnerNoSee);

	virtual void VisibleDeathPostProcess(const bool InEnabled);

	AAbstractWeapon* FindByIndexWeapon()
	{
		return GetInventoryComponent()->FindByIndexWeapon(WeaponCurrentIndex);
	}


public:
	virtual void Jump() override;
	virtual void StopJumping() override;
	virtual void Sprint() override;
	virtual void StopSprint() override;
	virtual void OnCrouch() override;

protected:
	virtual void TurnAtRate(float Rate) override;
	virtual void LookUpAtRate(float Rate) override;
	virtual void MoveForward(float Value) override;
	virtual void MoveRight(float Value) override;
	virtual void ReleaseObjects() override;
	virtual void PickupObjects() override;
	virtual void OnWalkAction() override;

public:
	virtual void StartRagdollAction() override;

protected:
	void SpawnBackPack();

	void OnFirePressed();
	void OnFireReleassed();
	void OnMeleeAttack();
	void OnReload();
	void OnChangeWeapon();
	void OnEquipWeapon();
	void Aiming();
	void StopAiming();

	// Apply to OnALSGaitChange_Implementation
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Player|ALS")
	void BP_UpdateCameraAction(class UCurveFloat* LerpCurve);

};

