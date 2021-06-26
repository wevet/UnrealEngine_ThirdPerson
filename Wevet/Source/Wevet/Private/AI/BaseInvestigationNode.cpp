// Copyright 2018 wevet works All Rights Reserved.

#include "AI/BaseInvestigationNode.h"


ABaseInvestigationNode::ABaseInvestigationNode(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = false;
}


void ABaseInvestigationNode::GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const
{
	TagContainer = GameplayTagContainer;
}
