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
	if (AAIControllerBase* Controller = Cast<AAIControllerBase>(OwnerComp.GetAIOwner()))
	{
		if (AWayPointBase* WayPoint = Controller->GetWayPoint())
		{
			AAICharacterBase* Character = Cast<AAICharacterBase>(Controller->GetPawn());
			if (Character)
			{
				const float SearchRadius = IAIPawnOwner::Execute_GetAttackTraceForwardDistance(Character);
				const FVector SearchOrigin = WayPoint->GetActorLocation();
				UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetNavigationSystem(Controller);
				if (!NavSystem)
				{
					return EBTNodeResult::Failed;
				}

				FNavLocation ResultLocation;
				const bool bResult = NavSystem->GetRandomPointInNavigableRadius(SearchOrigin, SearchRadius, ResultLocation);
				if (bResult)
				{
					Controller->SetBlackboardPatrolLocation(ResultLocation.Location);
					return EBTNodeResult::Succeeded;
				}
			}
		}
	}
	return EBTNodeResult::Failed;
}
