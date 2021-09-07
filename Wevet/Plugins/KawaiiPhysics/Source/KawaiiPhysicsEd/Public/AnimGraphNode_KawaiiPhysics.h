#pragma once

#include "AnimGraphNode_Base.h"
#include "AnimNode_KawaiiPhysics.h"
#include "AnimGraphNode_SkeletalControlBase.h"
#include "EdGraph/EdGraphNodeUtils.h"
#include "AnimGraphNode_KawaiiPhysics.generated.h"


class FCompilerResultsLog;

UCLASS()
class UAnimGraphNode_KawaiiPhysics : public UAnimGraphNode_SkeletalControlBase
{
	GENERATED_UCLASS_BODY()

public:
	UPROPERTY(EditAnywhere, Category = Settings)
	FAnimNode_KawaiiPhysics Node;

	UPROPERTY(EditAnywhere, Category = Debug)
	bool bEnableDebugDrawBone = true;

	UPROPERTY(EditAnywhere, Category = Debug)
	bool bEnableDebugBoneLengthRate = true;

	UPROPERTY(EditAnywhere, Category = Debug)
	bool bEnableDebugDrawAngleLimit = true;

	UPROPERTY(EditAnywhere, Category = Debug)
	bool bEnableDebugDrawSphereLimit = true;

	UPROPERTY(EditAnywhere, Category = Debug)
	bool bEnableDebugDrawCapsuleLimit = true;

	UPROPERTY(EditAnywhere, Category = Debug)
	bool bEnableDebugDrawPlanerLimit = true;

public:
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;

protected:

	virtual FEditorModeID GetEditorMode() const;
	virtual void ValidateAnimNodePostCompile(FCompilerResultsLog& MessageLog, UAnimBlueprintGeneratedClass* CompiledClass, int32 CompiledNodeIndex) override;
	virtual FText GetControllerDescription() const override;
	virtual const FAnimNode_SkeletalControlBase* GetNode() const override { return &Node; }
	virtual void Draw(FPrimitiveDrawInterface* PDI, USkeletalMeshComponent* SkelMeshComp) const override;

private:
	FNodeTitleTextTable CachedNodeTitles;
};