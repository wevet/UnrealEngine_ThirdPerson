// Copyright 2018 wevet works All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "Perception/AISenseConfig_Prediction.h"
#include "BehaviorTree/BehaviorTree.h"
#include "Wevet.h"
#include "WevetTypes.h"
#include "AIControllerBase.generated.h"

class UBehaviorTreeComponent;
class AAICharacterBase;
class AWayPointBase;

class UBehaviorTreeComponent;
class UBlackboardComponent;
class UAIPerceptionComponent;


UCLASS(ABSTRACT)
class WEVET_API AAIControllerBase :  public AAIController
{
	GENERATED_BODY()

public:
	AAIControllerBase(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnUnPossess() override;
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
	AWayPointBase* GetRandomAtWayPoint();
	virtual void SetBlackboardTarget(APawn* NewTarget);
	virtual void SetBlackboardWayPoint(AWayPointBase* NewWayPoint);
	virtual void SetBlackboardBotType(EBotBehaviorType NewType);
	virtual void SetBlackboardSeeActor(const bool NewCanSeeActor);
	virtual void SetBlackboardHearActor(const bool NewCanHearActor);
	virtual void SetBlackboardPatrolLocation(const FVector NewLocation);
	virtual void SetBlackboardActionState(const EAIActionState NewAIActionState);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components, meta = (AllowPrivateAccess = "true"))
	class UBehaviorTreeComponent * BehaviorTreeComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components, meta = (AllowPrivateAccess = "true"))
	class UBlackboardComponent* BlackboardComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components, meta = (AllowPrivateAccess = "true"))
	class UAIPerceptionComponent* AIPerceptionComponent;

	UFUNCTION(BlueprintCallable, Category = "AIControllerBase|Perception")
	virtual void OnTargetPerceptionUpdatedRecieve(AActor* Actor, FAIStimulus Stimulus);

	AAICharacterBase* AICharacterOwner;
	TArray<AWayPointBase*> WayPointList;

	class UAISenseConfig_Sight* SightConfig;
	class UAISenseConfig_Hearing* HearConfig;
	class UAISenseConfig_Prediction* PredictionConfig;

	UPROPERTY(EditDefaultsOnly, Category = "AIControllerBase|Variable")
	FName CanSeePlayerKeyName;

	UPROPERTY(EditDefaultsOnly, Category = "AIControllerBase|Variable")
	FName CanHearPlayerKeyName;

	UPROPERTY(EditDefaultsOnly, Category = "AIControllerBase|Variable")
	FName TargetKeyName;

	UPROPERTY(EditDefaultsOnly, Category = "AIControllerBase|Variable")
	FName PatrolLocationKeyName;

	UPROPERTY(EditDefaultsOnly, Category = "AIControllerBase|Variable")
	FName BotTypeKeyName;

	UPROPERTY(EditDefaultsOnly, Category = "AIControllerBase|Variable")
	FName ActionStateKeyName;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AIControllerBase|Variable")
	TArray<FVector> PointsArray;

public:
	UFUNCTION(BlueprintCallable, Category = "AIControllerBase|Function")
	const TArray<FVector>& GetPathPointArray();
};
