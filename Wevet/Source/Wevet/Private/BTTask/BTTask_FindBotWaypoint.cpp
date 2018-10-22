// Fill out your copyright notice in the Description page of Project Settings.

#include "BTTask_FindBotWaypoint.h"

#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyAllTypes.h"
#include "AIControllerBase.h"
#include "WayPointBase.h"
#include "Engine.h"

EBTNodeResult::Type UBTTask_FindBotWaypoint::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIControllerBase* AIController = Cast<AAIControllerBase>(OwnerComp.GetAIOwner());

	if (AIController)
	{
		AActor* NewWaypoint = nullptr;

		TArray<AActor*> AllWaypoints;
		UGameplayStatics::GetAllActorsOfClass(AIController, AWayPointBase::StaticClass(), AllWaypoints);

		if (AllWaypoints.Num() <= 0)
		{
			return EBTNodeResult::Failed;
		}

		NewWaypoint = AllWaypoints[FMath::RandRange(0, AllWaypoints.Num() - 1)];

		if (NewWaypoint)
		{
			OwnerComp.GetBlackboardComponent()->SetValue<UBlackboardKeyType_Object>(BlackboardKey.GetSelectedKeyID(), NewWaypoint);
			return EBTNodeResult::Succeeded;
		}
	}

	return EBTNodeResult::Failed;
}
