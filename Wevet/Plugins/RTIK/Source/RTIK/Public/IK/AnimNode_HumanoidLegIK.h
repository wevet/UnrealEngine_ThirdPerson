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

public:
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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings, meta = (PinHiddenByDefault))
	bool bEnable;	

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Solver, meta = (PinHiddenByDefault))
	EHumanoidLegIKMode Mode;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Solver)
	EHumanoidLegIKSolver Solver;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Solver, meta = (PinHiddenByDefault))
	TEnumAsByte<EBoneRotationSource> EffectorRotationSource;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings)
	float EffectorVelocity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings)
	bool bEffectorMovesInstantly;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings)
	float MinimumEffectorDelta;
	
public:
	FAnimNode_HumanoidLegIK()
		:
		FootTargetWorld(FVector(0.0f, 0.0f, 0.0f)),
		Precision(0.001f),
		MaxIterations(10),
		bEnable(true),
		Mode(EHumanoidLegIKMode::IK_Human_Leg_Locomotion),
		Solver(EHumanoidLegIKSolver::IK_Human_Leg_Solver_FABRIK),
		EffectorRotationSource(EBoneRotationSource::BRS_KeepComponentSpaceRotation),
		EffectorVelocity(300.0f),
		bEffectorMovesInstantly(false),
		LastEffectorOffset(0.0f, 0.0f, 0.0f)
	{
		bEnableDebugDraw = false;
		DeltaTime = 0.0f;
	}

	// Node初期化
	virtual void Initialize_AnyThread(const FAnimationInitializeContext& Context) override
	{
		Super::Initialize_AnyThread(Context);
		BaseComponentPose.Initialize(Context);
	}

	// すべてのボーンをキャッシュするために呼び出す
	virtual void CacheBones_AnyThread(const FAnimationCacheBonesContext& Context) override
	{
		Super::CacheBones_AnyThread(Context);
		BaseComponentPose.CacheBones(Context);
	}

	// 更新処理
	virtual void UpdateInternal(const FAnimationUpdateContext& Context) override
	{
		Super::UpdateInternal(Context);
		BaseComponentPose.Update(Context);
		DeltaTime = Context.GetDeltaTime();
	}

	// 影響を受けるボーンの新しいコンポーネント空間変換を評価
	virtual void EvaluateSkeletalControl_AnyThread(FComponentSpacePoseContext& Output, TArray<FBoneTransform>& OutBoneTransforms) override;

	// ノードの処理を行うかどうかの判定
	virtual bool IsValidToEvaluate(const USkeleton* Skeleton, const FBoneContainer& RequiredBones) override
	{
		if (Leg == nullptr || TraceData == nullptr)
		{
#if ENABLE_IK_DEBUG_VERBOSE
			UE_LOG(LogRTIK, Error, TEXT("An input wrapper object was null : %s"), *FString(__FUNCTION__));
#endif
			return false;
		}
		bool bValid = Leg->InitIfInvalid(RequiredBones);
#if ENABLE_IK_DEBUG_VERBOSE
		if (!bValid)
		{
			UE_LOG(LogRTIK, Warning, TEXT("Could not initialize : %s"), *FString(__FUNCTION__));
		}
#endif
		return bValid;
	}

	// ボーンの参照の初期化
	virtual void InitializeBoneReferences(const FBoneContainer& RequiredBones) override
	{
		if (Leg == nullptr || TraceData == nullptr)
		{
#if ENABLE_IK_DEBUG
			UE_LOG(LogRTIK, Error, TEXT("An input wrapper object was null : %s"), *FString(__FUNCTION__));
#endif
			return;
		}
		if (!Leg->InitBoneReferences(RequiredBones))
		{
#if ENABLE_IK_DEBUG
			UE_LOG(LogRTIK, Warning, TEXT("Could not initialize : %s"), *FString(__FUNCTION__));
#endif
		}
	}

protected:
	float DeltaTime;
	FVector LastEffectorOffset;
};
