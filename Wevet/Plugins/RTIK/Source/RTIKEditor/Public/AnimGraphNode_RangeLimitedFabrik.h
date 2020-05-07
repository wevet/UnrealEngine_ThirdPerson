
#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "AnimGraphNode_SkeletalControlBase.h"
#include "IK/AnimNode_RangeLimitedFabrik.h"
#include "AnimGraphNode_RangeLimitedFabrik.generated.h"

class FPrimitiveDrawInterface;
class USkeletalMeshComponent;

UCLASS()
class RTIKEDITOR_API UAnimGraphNode_RangeLimitedFabrik : public UAnimGraphNode_SkeletalControlBase
{
	GENERATED_BODY()

public:

	FText GetNodeTitle(ENodeTitleType::Type TitleType) const override
	{
		return FText::FromString(FString("Range Limited FABRIK"));
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
		return FText::FromString(FString("FABRIK solver with range limits"));
	}

protected:
	UPROPERTY(EditAnywhere, Category = Settings)
	FAnimNode_RangeLimitedFabrik Node;
};
