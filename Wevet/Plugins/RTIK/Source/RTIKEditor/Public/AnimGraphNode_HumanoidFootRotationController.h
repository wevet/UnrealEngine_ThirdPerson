
#pragma once

#include "AnimGraphNode_SkeletalControlBase.h"
#include "IK/AnimNode_HumanoidFootRotationController.h"
#include "AnimGraphNode_HumanoidFootRotationController.generated.h"

UCLASS()
class RTIKEDITOR_API UAnimGraphNode_HumanoidFootRotationController : public UAnimGraphNode_SkeletalControlBase
{
	GENERATED_BODY()
	
public:

	FText GetNodeTitle(ENodeTitleType::Type TitleType) const override
	{
		return FText::FromString(FString("Humanoid Foot Rotation Controller"));
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
		return FText::FromString(FString("Rotate a Humanoid FoottoMatch the Slope of the Floor"));		
	}

protected:
	UPROPERTY(EditAnywhere, Category = Settings)
	FAnimNode_HumanoidFootRotationController Node;

};