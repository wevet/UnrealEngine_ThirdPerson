// Fill out your copyright notice in the Description page of Project Settings.

#include "BTTask_FindPatrolLocation.h"

#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyAllTypes.h"
#include "AIControllerBase.h"
#include "WayPointBase.h"
#include "Engine.h"
//#include "NavigationSystem.h"

EBTNodeResult::Type UBTTask_FindPatrolLocation::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIControllerBase* AIController = Cast<AAIControllerBase>(OwnerComp.GetAIOwner());

	if (AIController)
	{
		AWayPointBase* WayPoint = AIController->GetRandomAtWayPoint();
		if (WayPoint)
		{
			const float SearchRadius = 200.f;
			const FVector SearchOrigin = WayPoint->GetActorLocation();

			FNavLocation ResultLocation;

			//UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetNavigationSystem(AIController);
			//if (NavSystem && NavSystem->GetRandomPointInNavigableRadius(SearchOrigin, SearchRadius, ResultLocation))
			//{
			//	OwnerComp.GetBlackboardComponent()->SetValue<UBlackboardKeyType_Vector>(BlackboardKey.GetSelectedKeyID(), ResultLocation.Location);
			//	return EBTNodeResult::Succeeded;
			//}
		}
	}

	return EBTNodeResult::Failed;
}



