#pragma once

#include "CoreMinimal.h"
#include "IKTypes.h"
#include "HumanoidIK.h"
#include "BoneControllers/AnimNode_SkeletalControlBase.h"
#include "AnimNode_HumanoidLegIK.generated.h"


USTRUCT()
struct RTIK_API FAnimNode_HumanoidLegIK : public FAnimNode_SkeletalControlBase
{
	GENERATED_USTRUCT_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Links)
	FComponentSpacePoseLink BaseComponentPose;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Bones, meta = (PinShownByDefault))
	UHumanoidLegChain_Wrapper* Leg;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Bones, meta = (PinShownByDefault))
	UHumanoidIKTraceData_Wrapper* TraceData;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Bones, meta = (PinShownByDefault))
	FTransform FootTargetWorld;
		
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings)
	bool bEnableDebugDraw;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Solver)
	float Precision;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Solver)
	int32 MaxIterations;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Solver, meta = (PinHiddenByDefault))
	EHumanoidLegIKMode Mode;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Solver)
	EHumanoidLegIKSolver Solver;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings)
	float EffectorVelocity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings)
	bool bEffectorMovesInstantly;
	
public:
	FAnimNode_HumanoidLegIK() : Super()
	{
		Mode = EHumanoidLegIKMode::IK_Human_Leg_Locomotion;
		Solver = EHumanoidLegIKSolver::IK_Human_Leg_Solver_FABRIK;
		FootTargetWorld = FTransform::Identity;
		LastEffectorOffset = FVector::ZeroVector;
		Precision = 0.001f;
		MaxIterations = 10;
		EffectorVelocity = 250.0f;
		bEnableDebugDraw = false;
		bEffectorMovesInstantly = false;
		DeltaTime = 0.0f;
	}

	virtual void Initialize_AnyThread(const FAnimationInitializeContext& Context) override
	{
		Super::Initialize_AnyThread(Context);
		BaseComponentPose.Initialize(Context);
	}

	virtual void CacheBones_AnyThread(const FAnimationCacheBonesContext& Context) override
	{
		Super::CacheBones_AnyThread(Context);
		BaseComponentPose.CacheBones(Context);
	}

	virtual void UpdateInternal(const FAnimationUpdateContext& Context) override
	{
		Super::UpdateInternal(Context);
		BaseComponentPose.Update(Context);
		DeltaTime = Context.GetDeltaTime();
	}

	virtual bool IsValidToEvaluate(const USkeleton* Skeleton, const FBoneContainer& RequiredBones) override
	{
		if (Leg == nullptr || TraceData == nullptr)
		{
			return false;
		}
		return Leg->InitIfInvalid(RequiredBones);
	}

	virtual void InitializeBoneReferences(const FBoneContainer& RequiredBones) override
	{
		if (Leg == nullptr || TraceData == nullptr)
		{
			return;
		}
		if (!Leg->InitBoneReferences(RequiredBones))
		{
			//
		}
	}

	virtual void EvaluateSkeletalControl_AnyThread(FComponentSpacePoseContext& Output, TArray<FBoneTransform>& OutBoneTransforms) override;

protected:
	float DeltaTime;
	FVector LastEffectorOffset;
};
