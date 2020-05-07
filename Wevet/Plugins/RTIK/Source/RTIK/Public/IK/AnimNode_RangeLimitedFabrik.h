#pragma once

#include "CoreMinimal.h"
#include "IKTypes.h"
#include "HumanoidIK.h"
#include "BoneControllers/AnimNode_SkeletalControlBase.h"
#include "AnimNode_RangeLimitedFabrik.generated.h"


USTRUCT()
struct RTIK_API FAnimNode_RangeLimitedFabrik : public FAnimNode_SkeletalControlBase
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = EndEffector, meta = (PinShownByDefault))
	FTransform EffectorTransform;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = EndEffector)
	TEnumAsByte<enum EBoneControlSpace> EffectorTransformSpace;

	UPROPERTY(EditAnywhere, Category = EndEffector)
	FBoneReference EffectorTransformBone;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Bones, meta = (PinShownByDefault))
	URangeLimitedIKChainWrapper* IKChain;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = EndEffector)
	TEnumAsByte<enum EBoneRotationSource> EffectorRotationSource;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Solver, meta = (PinShownByDefault))
	ERangeLimitedFABRIKSolverMode SolverMode;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Solver)
	float Precision;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Solver)
	int32 MaxIterations;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Solver)
	float MaxRootDragDistance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Solver, meta = (UIMin = 0.0f))
	float RootDragStiffness;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings)
	bool bEnableDebugDraw;

public:
	FAnimNode_RangeLimitedFabrik()
		:
		EffectorTransform(FTransform::Identity),
		EffectorTransformSpace(BCS_ComponentSpace),
		EffectorRotationSource(BRS_KeepLocalSpaceRotation),
		SolverMode(ERangeLimitedFABRIKSolverMode::RLF_Normal),
		Precision(1.f),
		MaxIterations(10),
		MaxRootDragDistance(0.0f),
		RootDragStiffness(1.0f),
		bEnableDebugDraw(false)
	{ }

	// GameThreadでdebug処理
	virtual void GatherDebugData(FNodeDebugData& DebugData) override
	{
		FString DebugLine = DebugData.GetNodeName(this);
		DebugData.AddDebugItem(DebugLine);
		ComponentPose.GatherDebugData(DebugData);
	}

	// 影響を受けるボーンの新しいコンポーネント空間変換を評価
	virtual void EvaluateSkeletalControl_AnyThread(FComponentSpacePoseContext& Output, TArray<FBoneTransform>& OutBoneTransforms) override;

	// ノードの処理を行うかどうかの判定
	virtual bool IsValidToEvaluate(const USkeleton* Skeleton, const FBoneContainer& RequiredBones) override
	{
		if (IKChain == nullptr)
		{
#if ENABLE_IK_DEBUG_VERBOSE
			UE_LOG(LogRTIK, Error, TEXT("An input wrapper object was null : %s"), *FString(__FUNCTION__));
#endif
			return false;
		}
		if (IKChain->Chain.Num() < 2)
		{
			return false;
		}
		return (Precision > 0 && IKChain->IsValid(RequiredBones));
	}

	// ボーンの参照の初期化
	virtual void InitializeBoneReferences(const FBoneContainer& RequiredBones) override
	{
		if (IKChain == nullptr)
		{
#if ENABLE_IK_DEBUG
			UE_LOG(LogRTIK, Error, TEXT("An input wrapper object was null : %s"), *FString(__FUNCTION__));
#endif
			return;
		}
		IKChain->InitIfInvalid(RequiredBones);
		size_t NumBones = IKChain->Chain.Num();
		if (NumBones < 2)
		{
			return;
		}
		EffectorTransformBone = IKChain->Chain[NumBones - 1].BoneRef;
		EffectorTransformBone.Initialize(RequiredBones);
	}

protected:
	// Update rotation of parent bone to reflect new position of the child. 
	void UpdateParentRotation(
		FTransform& ParentTransform, 
		const FIKBone& ParentBone, 
		FTransform& ChildTransform, 
		const FIKBone& ChildBone, 
		FCSPose<FCompactPose>& Pose) const;

#if WITH_EDITOR
	// Cached CS location when in editor for debug drawing
	FTransform CachedEffectorCSTransform;
#endif
};
