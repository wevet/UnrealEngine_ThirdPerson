#pragma once

#include "CoreMinimal.h"
#include "IKTypes.h"
#include "HumanoidIK.h"
#include "BoneControllers/AnimNode_SkeletalControlBase.h"
#include "Engine/SkeletalMeshSocket.h"
#include "AnimNode_RangeLimitedFabrik.generated.h"


USTRUCT()
struct RTIK_API FAnimNode_RangeLimitedFabrik : public FAnimNode_SkeletalControlBase
{
	GENERATED_USTRUCT_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = EndEffector, meta = (PinShownByDefault))
	FTransform EffectorTransform;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = EndEffector)
	TEnumAsByte<enum EBoneControlSpace> EffectorTransformSpace;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = EndEffector)
	TEnumAsByte<enum EBoneRotationSource> EffectorRotationSource;

	UPROPERTY(EditAnywhere, Category = EndEffector)
	FBoneReference EffectorTransformBone;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Bones, meta = (PinShownByDefault))
	class URangeLimitedIKChainWrapper* IKChain;

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
	FAnimNode_RangeLimitedFabrik() : Super(),
		EffectorTransformSpace(BCS_ComponentSpace),
		EffectorRotationSource(BRS_KeepLocalSpaceRotation),
		SolverMode(ERangeLimitedFABRIKSolverMode::RLF_Normal)
	{
		EffectorTransform = FTransform::Identity;
		MaxIterations = 10;
		Precision = 1.0f;
		MaxRootDragDistance = 0.0f;
		RootDragStiffness = 1.0f;
		bEnableDebugDraw = false;
	}


	virtual void GatherDebugData(FNodeDebugData& DebugData) override
	{
		FString DebugLine = DebugData.GetNodeName(this);
		DebugData.AddDebugItem(DebugLine);
		ComponentPose.GatherDebugData(DebugData);
	}


	virtual bool IsValidToEvaluate(const USkeleton* Skeleton, const FBoneContainer& RequiredBones) override
	{
		if (IKChain == nullptr)
		{
			return false;
		}
		if (IKChain->GetChain().Num() < 2)
		{
			return false;
		}
		return (Precision > 0 && IKChain->IsValid(RequiredBones));
	}


	virtual void InitializeBoneReferences(const FBoneContainer& RequiredBones) override
	{
		if (IKChain == nullptr)
		{
			return;
		}
		IKChain->InitIfInvalid(RequiredBones);

		const size_t NumBones = IKChain->GetChain().Num();
		if (NumBones < 2)
		{
			return;
		}
		const FIKBone Bone = IKChain->GetChain()[NumBones - 1];
		EffectorTransformBone = Bone.BoneRef;
		EffectorTransformBone.Initialize(RequiredBones);
	}


	virtual void EvaluateSkeletalControl_AnyThread(FComponentSpacePoseContext& Output, TArray<FBoneTransform>& OutBoneTransforms) override;

protected:
	void UpdateParentRotation(FTransform& ParentTransform, const FIKBone& ParentBone, FTransform& ChildTransform, const FIKBone& ChildBone, FCSPose<FCompactPose>& Pose) const
	{
		FTransform OldParentTransform = Pose.GetComponentSpaceTransform(ParentBone.BoneIndex);
		FTransform OldChildTransform = Pose.GetComponentSpaceTransform(ChildBone.BoneIndex);
		FVector OldDir = (OldChildTransform.GetLocation() - OldParentTransform.GetLocation()).GetUnsafeNormal();

		FVector NewDir = (ChildTransform.GetLocation() - ParentTransform.GetLocation()).GetUnsafeNormal();
		FVector RotationAxis = FVector::CrossProduct(OldDir, NewDir).GetSafeNormal();
		float RotationAngle = FMath::Acos(FVector::DotProduct(OldDir, NewDir));
		FQuat DeltaRotation = FQuat(RotationAxis, RotationAngle);

		checkSlow(DeltaRotation.IsNormalized());
		ParentTransform.SetRotation(DeltaRotation * OldParentTransform.GetRotation());
		ParentTransform.NormalizeRotation();
	}

#if WITH_EDITOR
	// Cached CS location when in editor for debug drawing
	FTransform CachedEffectorCSTransform;
#endif
};
