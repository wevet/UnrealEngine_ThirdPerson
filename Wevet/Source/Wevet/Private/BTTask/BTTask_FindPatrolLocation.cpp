// Copyright 2018 wevet works All Rights Reserved.

#include "BTTask_FindPatrolLocation.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyAllTypes.h"
#include "AI/AIControllerBase.h"
#include "AI/AICharacterBase.h"
#include "AI/WayPointBase.h"
#include "Engine.h"
#include "NavigationSystem.h"
#include "Interface/AIPawnOwner.h"

EBTNodeResult::Type UBTTask_FindPatrolLocation::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (AAIControllerBase* AIController = Cast<AAIControllerBase>(OwnerComp.GetAIOwner()))
	{
		if (AWayPointBase* WayPoint = AIController->GetRandomAtWayPoint())
		{
			APawn* AIPawn = AIController->GetPawn();
			IAIPawnOwner* Interface = Cast<IAIPawnOwner>(AIPawn);
			if (Interface)
			{
				const float SearchRadius = IAIPawnOwner::Execute_GetAttackTraceForwardDistance(Interface->_getUObject());
				const FVector SearchOrigin = WayPoint->GetActorLocation();
				FNavLocation ResultLocation;

				UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetNavigationSystem(AIController);
				if (NavSystem && NavSystem->GetRandomPointInNavigableRadius(SearchOrigin, SearchRadius, ResultLocation))
				{
					AIController->SetBlackboardPatrolLocation(ResultLocation.Location);
					return EBTNodeResult::Succeeded;
				}
			}
		}
	}
	return EBTNodeResult::Failed;
}
