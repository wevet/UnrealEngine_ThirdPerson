#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "Engine/TargetPoint.h"
#include "EdGraph/EdGraphNodeUtils.h"
#include "AnimGraphNode_SkeletalControlBase.h"
#include "AnimNode_FullbodyIK.h"
#include "AnimGraphNode_FullbodyIK.generated.h"

// actor class used for bone selector
#define ABoneSelectActor ATargetPoint

class FFullbodyIKDelegate;
class IDetailLayoutBuilder;

UCLASS(MinimalAPI)
class UAnimGraphNode_FullbodyIK : public UAnimGraphNode_SkeletalControlBase
{
	GENERATED_UCLASS_BODY()

	UPROPERTY(EditAnywhere, Category=Settings)
	FAnimNode_FullbodyIK Node;

	/** Enable drawing of the debug information of the node */
	UPROPERTY(EditAnywhere, Category=Debug)
	bool bEnableDebugDraw;

public:
	virtual void Serialize(FArchive& Ar) override;

	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	virtual FText GetTooltipText() const override;

	virtual void CustomizeDetails(class IDetailLayoutBuilder& DetailBuilder) override;
	virtual FEditorModeID GetEditorMode() const;
	virtual void CopyNodeDataToPreviewNode(FAnimNode_Base* InPreviewNode) override;
	virtual void CopyPinDefaultsToNodeData(UEdGraphPin* InPin) override;

	virtual const FAnimNode_SkeletalControlBase* GetNode() const override { return &Node; }

protected:
	virtual void Draw(FPrimitiveDrawInterface* PDI, USkeletalMeshComponent* SkelMeshComp) const override;
	virtual FText GetControllerDescription() const override;

private:
	FNodeTitleTextTable CachedNodeTitles;
};
