// Copyright 2018 wevet works All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "BehaviorTree/BehaviorTree.h"
#include "Macros.h"
#include "STypes.h"
#include "AICombatExecuter.h"
#include "AIControllerBase.generated.h"

class UBehaviorTreeComponent;
class AAICharacterBase;
class AWayPointBase;

class UBehaviorTreeComponent;
class UBlackboardComponent;
class UAIPerceptionComponent;


UCLASS(ABSTRACT)
class WEVET_API AAIControllerBase :  public AAIController, public IAICombatExecuter
{
	GENERATED_BODY()

public:
	AAIControllerBase(const FObjectInitializer& ObjectInitializer);
	virtual void Possess(APawn* Pawn) override;
	virtual void UnPossess() override;

protected:
	virtual void BeginPlay() override;
	FGenericTeamId GetGenericTeamId() const override;

public:
	FORCEINLINE class UBehaviorTreeComponent* GetBehaviorTreeComponent()
	{
		return BehaviorTreeComponent;
	}

	FORCEINLINE class UBlackboardComponent* GetBlackboardComponent()
	{
		return BlackboardComponent;
	}

	FORCEINLINE class UAIPerceptionComponent* GetAIPerceptionComponent()
	{
		return AIPerceptionComponent;
	}

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AAIControllerBase|Interface")
	void Patrolling();
	virtual void Patrolling_Implementation() override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AAIControllerBase|Interface")
	void CheckEnemySighting();
	virtual void CheckEnemySighting_Implementation() override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AAIControllerBase|Interface")
	void Hunting();
	virtual void Hunting_Implementation() override;

	UFUNCTION(BlueprintCallable, Category = "AAIControllerBase|Variable")
	AAICharacterBase* GetAICharacter() const
	{
		return this->AICharacterOwner;
	}

	UFUNCTION(BlueprintCallable, Category = "AAIControllerBase|Components")
	AWayPointBase* GetRandomAtWayPoint();

	virtual void SetTargetEnemy(APawn* NewTarget);
	virtual void SetBlackboardBotType(EBotBehaviorType NewType);
	virtual void SetBlackboardSeeActor(bool InCanSeeActor);

protected:
	class UBehaviorTreeComponent * BehaviorTreeComponent;
	class UBlackboardComponent* BlackboardComponent;
	class UAIPerceptionComponent* AIPerceptionComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AAIControllerBase|Variable")
	FName CanSeePlayerKey;

	UFUNCTION(BlueprintCallable, Category = "AAIControllerBase|Perception")
	virtual void OnTargetPerceptionUpdatedRecieve(AActor* Actor, FAIStimulus Stimulus);

	AAICharacterBase* AICharacterOwner;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AAIControllerBase|Variable")
	TArray<AWayPointBase*> WayPointList;

	class UAISenseConfig_Sight* SightConfig;
	class UAISenseConfig_Hearing* HearConfig;

	UPROPERTY(EditDefaultsOnly, Category = "AAIControllerBase|Variable")
	FName TargetEnemyKeyName;

	UPROPERTY(EditDefaultsOnly, Category = "AAIControllerBase|Variable")
	FName PatrolLocationKeyName;

	UPROPERTY(EditDefaultsOnly, Category = "AAIControllerBase|Variable")
	FName CurrentWaypointKeyName;

	UPROPERTY(EditDefaultsOnly, Category = "AAIControllerBase|Variable")
	FName BotTypeKeyName;
};
