// Fill out your copyright notice in the Description page of Project Settings.

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
	virtual void Die_Implementation() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AAICharacterBase|Variable")
	UPawnSensingComponent* PawnSensingComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AAICharacterBase|Variable")
	UWidgetComponent* WidgetComponent;

	UFUNCTION(BlueprintCallable, Category = "AAICharacterBase|Variable")
	virtual void SetTargetActor(AActor* Actor);

	UFUNCTION(BlueprintCallable, Category = "AAICharacterBase|Variable")
	virtual void SetEnemyFound(bool EnemyFound);

	UFUNCTION(BlueprintCallable, Category = "AAICharacterBase|Variable")
	virtual AActor* GetTarget()const
	{
		return this->Target;
	}

	UFUNCTION(BlueprintCallable, Category = "AAICharacterBase|Variable")
	virtual AController* GetController() const
	{
		return Cast<APawn>(GetTarget())->Controller;
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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AAICharacterBase|Variable")
	bool IsEnemyFound;

	UFUNCTION(BlueprintCallable, Category = "AAICharacterBase|Variable")
	bool GetEnemyFound() const
	{
		return this->IsEnemyFound;
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AAICharacterBase|Variable")
	class UBehaviorTree* BehaviorTree;

	UFUNCTION(BlueprintCallable, Category = "AAICharacterBase|Variable")
	virtual void InitializePosses();

protected:
	AActor* Target;

	virtual void BeginPlay() override;
	virtual FVector BulletTraceRelativeLocation() const override;
	virtual FVector BulletTraceForwardLocation() const override;

	UFUNCTION(BlueprintCallable, Category = "AAICharacterBase|PawnSensing")
	virtual void Scanning();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AAICharacterBase|Variable")
	TArray<AWayPointBase*> WayPointList;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AAICharacterBase|Variable")
	TSubclassOf<class AWeaponBase> SpawnWeapon;

	UFUNCTION(BlueprintCallable, Category = "AAICharacterBase|PawnSensing")
	virtual	void OnSeePawnRecieve(APawn* OtherPawn);

	UFUNCTION(BlueprintCallable, Category = "AAICharacterBase|PawnSensing")
	virtual	void OnHearNoiseRecieve(APawn *OtherActor, const FVector &Location, float Volume);
};
