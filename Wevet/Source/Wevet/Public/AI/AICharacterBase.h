// Copyright 2018 wevet works All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CharacterBase.h"
#include "Components/WidgetComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Perception/PawnSensingComponent.h"
#include "AICharacterBase.generated.h"


class AWeaponBase;
class AWayPointBase;
class AAIControllerBase;

UCLASS(ABSTRACT)
class WEVET_API AAICharacterBase : public ACharacterBase
{
	GENERATED_BODY()

public:
	AAICharacterBase(const FObjectInitializer& ObjectInitializer);
	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void PostInitializeComponents() override;
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components, meta = (AllowPrivateAccess = "true"))
	class UPawnSensingComponent* PawnSensingComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components, meta = (AllowPrivateAccess = "true"))
	class UWidgetComponent* WidgetComponent;

public:
	virtual void Die_Implementation() override;
	virtual void Equipment_Implementation() override;
	virtual void UnEquipment_Implementation() override;
	virtual void OnTakeDamage_Implementation(FName BoneName, float Damage, AActor* Actor) override;
	virtual void CreateWayPointList(TArray<AWayPointBase*>& OutWayPointList);
	virtual void CreateWeaponInstance();

	FORCEINLINE class UPawnSensingComponent* GetPawnSensingComponent() const
	{
		return PawnSensingComponent;
	}

	FORCEINLINE class UWidgetComponent* GetWidgetComponent() const
	{
		return WidgetComponent;
	}

	UFUNCTION(BlueprintCallable, Category = "AAICharacterBase|Variable")
	bool HasEnemyFound() const;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AAICharacterBase|Variable")
	class UBehaviorTree* BehaviorTree;

	UFUNCTION(BlueprintCallable, Category = "AAICharacterBase|Variable")
	virtual void InitializePosses();

	// @NOTE
	// for Animation Blueprint
	UFUNCTION(BlueprintCallable, Category = "AAICharacterBase|Target")
	ACharacterBase* GetTargetCharacter() const;
	virtual FVector BulletTraceRelativeLocation() const override;
	virtual FVector BulletTraceForwardLocation() const override;
	virtual void MainLoop(float DeltaTime);
	virtual void SetSeeTargetActor(ACharacterBase* const NewCharacter);
	virtual void SetHearTargetActor(AActor* const OtherActor);

protected:
	ACharacterBase* TargetCharacter;
	AAIControllerBase* AIController;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AAICharacterBase|Variable")
	TSubclassOf<class AWeaponBase> SpawnWeapon;

	UFUNCTION(BlueprintCallable, Category = "AAICharacterBase|PawnSensing")
	virtual	void OnSeePawnRecieve(APawn* OtherPawn);

	UFUNCTION(BlueprintCallable, Category = "AAICharacterBase|PawnSensing")
	virtual	void OnHearNoiseRecieve(APawn *OtherActor, const FVector &Location, float Volume);

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AAICharacterBase|Variable")
	float BulletDelay;

	/* Last bullet action after interval */
	float BulletInterval;

	/* Last time the player was spotted */
	float LastSeenTime;

	/* Last time the player was heard */
	float LastHeardTime;

	/* Last time we attacked something */
	float LastMeleeAttackTime;

	/* Time-out value to clear the sensed position of the player. Should be higher than Sense interval in the PawnSense component not never miss sense ticks. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AAICharacterBase|Variable")
	float SenseTimeOut;

	/* Resets after sense time-out to avoid unnecessary clearing of target each tick */
	bool bSeeTarget;
	bool bHearTarget;
};
