// Copyright 2018 wevet works All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Item/AbstractWeapon.h"
#include "Item/Trigger/NakedWeaponTrigger.h"
#include "NakedWeapon.generated.h"


/*
* Wrapper class ANakedWeaponTrigger
*/
UCLASS()
class WEVET_API ANakedWeapon : public AAbstractWeapon
{
	GENERATED_BODY()
	
public:
	ANakedWeapon(const FObjectInitializer& ObjectInitializer);
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason);

protected:
	virtual void BeginPlay() override;


public:
#pragma region Interface
	virtual void Take_Implementation(APawn* NewCharacter) override;
	virtual void DoFirePressed_Implementation() override;
	virtual void DoFireRelease_Implementation() override;
	virtual bool CanStrike_Implementation() const override;
	
	virtual void DoReload_Implementation() override
	{
		// Pass
	}


	virtual void DoReplenishment_Implementation(const int32 InAddAmmo) override
	{
		// Pass
	}
#pragma endregion


public:
	virtual bool CanReleaseItem() const override
	{
		return false;
	}

	virtual void ClearCollisionApply() override;


public:
	void NakedActionApply(const ENakedWeaponTriggerType NakedWeaponTriggerType, const bool Enable, bool& FoundResult);


protected:
	virtual void TakeDamageOuter(const FHitResult& HitResult) override;
	virtual	void RemoveDelegate() override;
	virtual void OnFirePressInternal() override;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ANakedWeapon|Asset")
	TArray<TSubclassOf<class ANakedWeaponTrigger>> TriggerTemplates;

	TMap<ENakedWeaponTriggerType, TArray<class ANakedWeaponTrigger*>> NakedWeaponTriggerMap;
	TArray<class ANakedWeaponTrigger*> NakedTriggerArray;


protected:
	void DoDeployTemplate();
	void AllClearNakedActionApply();

	UFUNCTION()
	void OnNakedTriggerHitDelegate(AActor* OtherActor, const FHitResult SweepResult);

	float GetAdditionalDamage() const;

	const static float GetAdditionalDamage(const ENakedWeaponTriggerType NakedWeaponTriggerType);

	const static FName GetAttachBoneName(const ENakedWeaponTriggerType NakedWeaponTriggerType);
};

