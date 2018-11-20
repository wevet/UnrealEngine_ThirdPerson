// Copyright 2018 wevet works All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CharacterBase.h"
#include "WeaponBase.h"
#include "WayPointBase.h"
#include "MockCharacter.h"
#include "Perception/AiPerceptionComponent.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "Perception/AISense_Hearing.h"
#include "Perception/PawnSensingComponent.h"
#include "AICharacterBase.generated.h"

/**
*
*/
UCLASS(ABSTRACT)
class WEVET_API AAICharacterBase : public ACharacterBase
{
	GENERATED_BODY()


public:
	AAICharacterBase(const FObjectInitializer& ObjectInitializer);

	virtual void OnConstruction(const FTransform& Transform) override;

	virtual void PostInitializeComponents() override;

	virtual void Tick(float DeltaTime) override;

	virtual void Die_Implementation() override;

	virtual void NotifyEquip_Implementation() override;

	virtual void OnTakeDamage_Implementation(FName BoneName, float Damage, AActor* Actor) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AAICharacterBase|Variable")
	UPawnSensingComponent* PawnSensingComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AAICharacterBase|Variable")
	UWidgetComponent* WidgetComponent;

	UFUNCTION(BlueprintCallable, Category = "AAICharacterBase|Variable")
	virtual void SetTargetActor(AActor* Actor);

	UFUNCTION(BlueprintCallable, Category = "AAICharacterBase|Variable")
	virtual AActor* GetTarget()const
	{
		return this->Target;
	}

	UFUNCTION(BlueprintCallable, Category = "AAICharacterBase|Variable")
	virtual AMockCharacter* GetPlayerCharacter() const;

	UFUNCTION(BlueprintCallable, Category = "AAICharacterBase|Variable")
	const TArray<AWayPointBase*>& GetWayPointList()
	{
		return this->WayPointList;
	}

	UFUNCTION(BlueprintCallable, Category = "AAICharacterBase|Variable")
	virtual void UpdateWeaponEvent();

	UFUNCTION(BlueprintCallable, Category = "AAICharacterBase|Variable")
	virtual void UpdateWayPointEvent();

	UFUNCTION(BlueprintCallable, Category = "AAICharacterBase|Variable")
	bool HasEnemyFound() const
	{
		if (Target)
		{
			return true;
		}
		return false;
	}

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AAICharacterBase|Variable")
	class UBehaviorTree* BehaviorTree;

	UFUNCTION(BlueprintCallable, Category = "AAICharacterBase|Variable")
	virtual void InitializePosses();

protected:
	AActor * Target;

	virtual void BeginPlay() override;
	virtual FVector BulletTraceRelativeLocation() const override;
	virtual FVector BulletTraceForwardLocation() const override;

	virtual void UpdateSensing();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AAICharacterBase|Variable")
	TArray<AWayPointBase*> WayPointList;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AAICharacterBase|Variable")
	TSubclassOf<class AWeaponBase> SpawnWeapon;

	UFUNCTION(BlueprintCallable, Category = "AAICharacterBase|PawnSensing")
	virtual	void OnSeePawnRecieve(APawn* OtherPawn);

	UFUNCTION(BlueprintCallable, Category = "AAICharacterBase|PawnSensing")
	virtual	void OnHearNoiseRecieve(APawn *OtherActor, const FVector &Location, float Volume);

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AAICharacterBase|Variable")
	float BulletDelay = 1.4f;

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
	bool bSensedTarget;
};
