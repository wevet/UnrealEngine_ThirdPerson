// Copyright 2018 wevet works All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "Perception/AISenseConfig_Prediction.h"
#include "Perception/AISenseConfig_Damage.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISenseConfig_Team.h"
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
	virtual FGenericTeamId GetGenericTeamId() const override
	{
		return TeamId;
	}

	virtual ETeamAttitude::Type GetTeamAttitudeTowards(const AActor& Other) const override;

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
	void SetBlackboardTarget(AActor* const NewTarget);
	void SetBlackboardSearchNodeHolder(AActor* const NewSearchNodeHolder);
	void SetBlackboardPatrolPointsHolder(AActor* const NewPatrolPointsHolder);
	void SetBlackboardPatrolLocation(const FVector NewLocation);
	void SetBlackboardDestinationLocation(const FVector NewDestination);
	void SetBlackboardActionState(const EAIActionState NewAIActionState);

	FORCEINLINE AActor* GetBlackboardTarget() const
	{
		return Cast<AActor>(BlackboardComponent->GetValueAsObject(TargetKeyName));
	}

	FORCEINLINE AActor* GetBlackboardSearchNodeHolder() const
	{
		return Cast<AActor>(BlackboardComponent->GetValueAsObject(SearchNodeHolderKeyName));
	}

	FORCEINLINE EAIActionState GetBlackboardActionState() const
	{
		return (EAIActionState)BlackboardComponent->GetValueAsEnum(ActionStateKeyName);
	}

	bool WasBlackboardTargetDeath() const;
	bool WasKilledCrew(AActor* const InActor) const;

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
	class UAISenseConfig_Damage* DamageConfig;

protected:
	class AAICharacterBase* Character;
	TArray<class AWayPointBase*> WayPointList;

	UPROPERTY(EditDefaultsOnly, Category = "AIController|Variable")
	struct FGenericTeamId TeamId;

	UPROPERTY(EditDefaultsOnly, Category = "AIController|Variable")
	struct FAIStimulus CurrentStimulus;

	UPROPERTY(EditDefaultsOnly, Category = "AIController|Variable")
	FName TargetKeyName;

	UPROPERTY(EditDefaultsOnly, Category = "AIController|Variable")
	FName PatrolLocationKeyName;

	UPROPERTY(EditDefaultsOnly, Category = "AIController|Variable")
	FName ActionStateKeyName;

	UPROPERTY(EditDefaultsOnly, Category = "AIController|Variable")
	FName SearchNodeHolderKeyName;

	UPROPERTY(EditDefaultsOnly, Category = "AIController|Variable")
	FName DestinationKeyName;

	UPROPERTY(EditDefaultsOnly, Category = "AIController|Variable")
	FName PatrolPointsHolderKeyName;

	UPROPERTY()
	TArray<FVector> PointsArray;

public:
	UFUNCTION(BlueprintCallable, Category = "AIController|Function")
	const TArray<FVector>& GetPathPointArray();

	UFUNCTION(BlueprintCallable, Category = "AIController|Function")
	void RemoveSearchNodeGenerator();

	UFUNCTION(BlueprintCallable, Category = "AIController|Function")
	void CheckTargetStatus(bool &OutResult);

	void StopTree();
	void ResumeTree();


protected:
	UFUNCTION()
	void OnDeath();

	UFUNCTION()
	void OnKill(AActor* InActor);

	void BattlePhaseUpdate();
};
