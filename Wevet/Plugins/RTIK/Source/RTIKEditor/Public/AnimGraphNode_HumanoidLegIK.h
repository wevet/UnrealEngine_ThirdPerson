
#pragma once

#include "AnimGraphNode_SkeletalControlBase.h"
#include "AnimNode_HumanoidLegIK.h"
#include "AnimGraphNode_HumanoidLegIK.generated.h"

UCLASS()
class RTIKEDITOR_API UAnimGraphNode_HumanoidLegIK : public UAnimGraphNode_SkeletalControlBase
{
	GENERATED_BODY()
	
public:
	FText GetNodeTitle(ENodeTitleType::Type TitleType) const override
	{
		return FText::FromString(FString("Humanoid Leg IK"));
	}

	FLinearColor GetNodeTitleColor() const override
	{
		return FLinearColor(0, 1, 1, 1);
	}
	
	FString GetNodeCategory() const override
	{
		return FString("IK Nodes");
	}
	
	virtual const FAnimNode_SkeletalControlBase* GetNode() const override 
	{
		return &Node; 
	}

protected:
	virtual FText GetControllerDescription() const
	{
		return FText::FromString(FString("IK a Humanoid TwoBone Leg to a Location"));
	}

protected:
	UPROPERTY(EditAnywhere, Category = Settings)
	FAnimNode_HumanoidLegIK Node;

};