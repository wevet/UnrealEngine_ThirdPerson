// Copyright 2018 wevet works All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "WevetTypes.h"
#include "CombatInstigator.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FCombatDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCombatOneDelegate, AActor* , InActor);

class AAbstractWeapon;
class UCharacterModel;

UINTERFACE(BlueprintType)
class WEVET_API UCombatInstigator : public UInterface
{
	GENERATED_UINTERFACE_BODY()
};

class WEVET_API ICombatInstigator
{
	GENERATED_IINTERFACE_BODY()

public:
	virtual FCombatDelegate* GetDeathDelegate() = 0;
	virtual FCombatDelegate* GetAliveDelegate() = 0;
	virtual FCombatOneDelegate* GetKillDelegate() = 0;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CombatInstigator")
	AActor* GetTarget() const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CombatInstigator")
	float GetMeleeDistance() const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CombatInstigator")
	void SetActionState(const EAIActionState InAIActionState);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CombatInstigator")
	void OnActionStateChange();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CombatInstigator")
	EAIActionState GetActionState() const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CombatInstigator")
	bool CanStrike() const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CombatInstigator")
	void InfrictionDamage(AActor* InfrictionActor, const bool bInfrictionDie);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CombatInstigator")
	bool IsDeath() const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CombatInstigator")
	bool IsStan() const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CombatInstigator")
	void Die();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CombatInstigator")
	void Alive();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CombatInstigator")
	void Equipment();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CombatInstigator")
	void UnEquipment();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CombatInstigator")
	UCharacterModel* GetPropertyModel() const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CombatInstigator")
	float MakeDamage(UCharacterModel* DamageModel, const int InWeaponDamage) const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CombatInstigator")
	bool CanKillDealDamage(const FName BoneName) const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CombatInstigator")
	void HitEffectReceive(const FHitResult& HitResult, const EGiveDamageType InGiveDamageType);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CombatInstigator")
	void DoFirePressed();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CombatInstigator")
	void DoFireReleassed();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CombatInstigator")
	void DoReload();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CombatInstigator")
	AAbstractWeapon* GetCurrentWeapon() const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CombatInstigator")
	FVector BulletTraceRelativeLocation() const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CombatInstigator")
	FVector BulletTraceForwardLocation() const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CombatInstigator")
	void FireActionMontage(float& OutFireDuration);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CombatInstigator")
	void ReloadActionMontage(float& OutReloadDuration);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CombatInstigator")
	void NakedAction(const ENakedWeaponTriggerType NakedWeaponTriggerType, const bool Enable, bool &FoundResult);
};
