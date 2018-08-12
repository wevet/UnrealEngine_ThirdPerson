// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "Macros.h"
#include "MockCharacter.h"
#include "AICharacterActorBase.h"
#include "WayPointBase.h"
#include "BulletBase.h"
#include "CombatControllerExecuter.h"
#include "AIControllerActorBase.generated.h"

/**
 * 
 */
UCLASS(ABSTRACT)
class WEVET_API AAIControllerActorBase :  public AAIController, public ICombatControllerExecuter
{
	GENERATED_BODY()
	
	
public:
	AAIControllerActorBase(const FObjectInitializer& ObjectInitializer);
	virtual void Possess(APawn* Pawn) override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AAIControllerActorBase|UCombatControllerExecuter")
	void Patrolling();
	virtual void Patrolling_Implementation() override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AAIControllerActorBase|UCombatControllerExecuter")
	void CheckEnemySighting();
	virtual void CheckEnemySighting_Implementation() override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AAIControllerActorBase|UCombatControllerExecuter")
	void Hunting();
	virtual void Hunting_Implementation() override;

	UFUNCTION(BlueprintCallable, Category = "AAIControllerActorBase|ReadOnlyValiable")
	AAICharacterActorBase* GetAICharacter() const 
	{
		return this->AICharacter;
	}

	UFUNCTION(BlueprintCallable, Category = "AAIControllerActorBase|ReadOnlyValiable")
	AWayPointBase* GetRandomAtWayPoint();

	UFUNCTION(BlueprintCallable, Category = "AAIControllerActorBase|ReadOnlyValiable")
	AMockCharacter* GetPlayerCharacter() const
	{
		return this->AICharacter->GetPlayerCharacter();
	}

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AAIControllerActorBase|Components")
	UAIPerceptionComponent* AIPerceptionComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AAIControllerActorBase|Valiable")
	FName CanSeePlayerKey;

	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, Category = "AAIControllerActorBase|Perception")
	virtual void OnTargetPerceptionUpdatedRecieve(AActor* Actor, FAIStimulus Stimulus);

	FGenericTeamId GetGenericTeamId() const override;

	AAICharacterActorBase* AICharacter;
	TArray<AWayPointBase*> WayPointList;

	class UAISenseConfig_Sight* SightConfig;
	class UAISenseConfig_Hearing* HearConfig;
};
