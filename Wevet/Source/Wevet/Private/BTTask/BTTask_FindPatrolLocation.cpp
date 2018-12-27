// Copyright 2018 wevet works All Rights Reserved.

#include "BTTask_FindPatrolLocation.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyAllTypes.h"
#include "AIControllerBase.h"
#include "WayPointBase.h"
#include "Engine.h"
#include "NavigationSystem.h"

EBTNodeResult::Type UBTTask_FindPatrolLocation::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	bool bSuccess = false;
	if (AAIControllerBase* AIController = Cast<AAIControllerBase>(OwnerComp.GetAIOwner()))
	{
		if (AWayPointBase* WayPoint = AIController->GetRandomAtWayPoint())
		{
			const float SearchRadius = 200.f;
			const FVector SearchOrigin = WayPoint->GetActorLocation();
			FNavLocation ResultLocation;

			UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetNavigationSystem(AIController);
			if (NavSystem && NavSystem->GetRandomPointInNavigableRadius(SearchOrigin, SearchRadius, ResultLocation))
			{
				AIController->SetBlackboardPatrolLocation(ResultLocation.Location);
				bSuccess = true;
			}
		}
	}
	return bSuccess ? EBTNodeResult::Succeeded : EBTNodeResult::Failed;
}
