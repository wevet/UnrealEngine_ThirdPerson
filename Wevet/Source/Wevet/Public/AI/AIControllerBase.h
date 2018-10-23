// Copyright 2018 wevet works All Rights Reserved.

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
#include "AICharacterBase.h"
#include "WayPointBase.h"
#include "AICombatControllerExecuter.h"
#include "AIControllerBase.generated.h"

/**
 *
 */

UCLASS(ABSTRACT)
class WEVET_API AAIControllerBase :  public AAIController, public IAICombatControllerExecuter
{
	GENERATED_BODY()


public:
	AAIControllerBase(const FObjectInitializer& ObjectInitializer);
	virtual void Possess(APawn* Pawn) override;

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

	UFUNCTION(BlueprintCallable, Category = "AAIControllerBase|Variable")
	AWayPointBase* GetRandomAtWayPoint();

	UFUNCTION(BlueprintCallable, Category = "AAIControllerBase|Variable")
	AMockCharacter* GetPlayerCharacter() const
	{
		return this->AICharacterOwner->GetPlayerCharacter();
	}

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AAIControllerBase|Components")
	UAIPerceptionComponent* AIPerceptionComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AAIControllerBase|Variable")
	FName CanSeePlayerKey;

	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, Category = "AAIControllerBase|Perception")
	virtual void OnTargetPerceptionUpdatedRecieve(AActor* Actor, FAIStimulus Stimulus);

	FGenericTeamId GetGenericTeamId() const override;

	AAICharacterBase* AICharacterOwner;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AAIControllerBase|Variable")
	TArray<AWayPointBase*> WayPointList;

	class UAISenseConfig_Sight* SightConfig;
	class UAISenseConfig_Hearing* HearConfig;
};
