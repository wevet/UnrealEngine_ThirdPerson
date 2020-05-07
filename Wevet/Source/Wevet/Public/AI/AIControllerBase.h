// Copyright 2018 wevet works All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "Perception/AISenseConfig_Prediction.h"
#include "Perception/AIPerceptionComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"

#include "Wevet.h"
#include "WevetTypes.h"
#include "AIControllerBase.generated.h"

class AAICharacterBase;
class AWayPointBase;


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

public:
	FORCEINLINE FGenericTeamId GetGenericTeamId() const override
	{
		return PTG_TEAM_ID_ENEMY;
	}

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
	class AWayPointBase* GetWayPoint() const;
	void SetBlackboardTarget(APawn* NewTarget);
	void SetBlackboardBotType(EBotBehaviorType NewType);
	void SetBlackboardSeeActor(const bool NewCanSeeActor);
	void SetBlackboardHearActor(const bool NewCanHearActor);
	void SetBlackboardPatrolLocation(const FVector NewLocation);
	void SetBlackboardActionState(const EAIActionState NewAIActionState);

	FORCEINLINE AActor* GetBlackboardTarget() const
	{
		return Cast<AActor>(BlackboardComponent->GetValueAsObject(TargetKeyName));
	}

	FORCEINLINE EAIActionState GetBlackboardActionState() const
	{
		return (EAIActionState)BlackboardComponent->GetValueAsEnum(ActionStateKeyName);
	}

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components, meta = (AllowPrivateAccess = "true"))
	class UBehaviorTreeComponent* BehaviorTreeComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components, meta = (AllowPrivateAccess = "true"))
	class UBlackboardComponent* BlackboardComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components, meta = (AllowPrivateAccess = "true"))
	class UAIPerceptionComponent* AIPerceptionComponent;

protected:
	UFUNCTION()
	virtual void OnTargetPerceptionUpdatedRecieve(AActor* Actor, FAIStimulus Stimulus);

	class UAISenseConfig_Sight* SightConfig;
	class UAISenseConfig_Hearing* HearConfig;
	class UAISenseConfig_Prediction* PredictionConfig;

protected:
	class AAICharacterBase* AICharacterOwner;
	TArray<class AWayPointBase*> WayPointList;

	UPROPERTY(EditDefaultsOnly, Category = "AIController|Variable")
	FName CanSeePlayerKeyName;

	UPROPERTY(EditDefaultsOnly, Category = "AIController|Variable")
	FName CanHearPlayerKeyName;

	UPROPERTY(EditDefaultsOnly, Category = "AIController|Variable")
	FName TargetKeyName;

	UPROPERTY(EditDefaultsOnly, Category = "AIController|Variable")
	FName PatrolLocationKeyName;

	UPROPERTY(EditDefaultsOnly, Category = "AIController|Variable")
	FName BotTypeKeyName;

	UPROPERTY(EditDefaultsOnly, Category = "AIController|Variable")
	FName ActionStateKeyName;

	UPROPERTY()
	TArray<FVector> PointsArray;

public:
	UFUNCTION(BlueprintCallable, Category = "AIController|Function")
	const TArray<FVector>& GetPathPointArray();

	void StopTree();
	void ResumeTree();

};
