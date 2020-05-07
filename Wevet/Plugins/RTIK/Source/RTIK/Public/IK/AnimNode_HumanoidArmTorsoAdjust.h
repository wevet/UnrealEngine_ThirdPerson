#pragma once

#include "CoreMinimal.h"
#include "IKTypes.h"
#include "HumanoidIK.h"
#include "BoneControllers/AnimNode_SkeletalControlBase.h"
#include "Engine/SkeletalMeshSocket.h"
#include "AnimNode_HumanoidArmTorsoAdjust.generated.h"

USTRUCT()
struct RTIK_API FAnimNode_HumanoidArmTorsoAdjust : public FAnimNode_SkeletalControlBase
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Links, meta = (PinShownByDefault))
	URangeLimitedIKChainWrapper* LeftArm;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Links, meta = (PinShownByDefault))
	URangeLimitedIKChainWrapper* RightArm;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings, meta = (PinShownByDefault))
	EHumanoidArmTorsoIKMode Mode;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings)
	bool bEnableDebugDraw;

	// FABRIKソルバーの精度 エフェクターがターゲットからこの距離内にある場合、反復は停止する
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Solver)
	float Precision;
	
	// FABRIK反復の最大数 この多くの反復の後、FABRIKは常に停止
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Solver)
	int32 MaxIterations;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings, meta = (PinHiddenByDefault))
	bool bEnable;	

	// Specifies a point that the torso should pivot around during upper-body IK. When the torso
	// bends forward / backward at the waist, this point will remain stationary, and the torso will
	// pivot around it. Should be in the middle of the torso, roughly halfway up the spine.
	// UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Torso)
	// FName TorsoPivotSocketName;

	// 肩の開始位置までの距離
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Torso, meta = (UIMin = 0.0f))
	float MaxShoulderDragDistance;

	// How much the shoulders will resist being moved from their original positions. 
	// Set above 1 to make the shoulders displace less; set below 1 to make them displace more (not recommended)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Torso, meta = (UIMin=0.01f))
	float ShoulderDragStiffness;
	
	// How far the torso may pitch forward, measured at the waist bone. In positive degrees.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Torso, meta = (UIMin=0.0f, UIMax = 180.0f))
	float MaxPitchForwardDegrees;

	// How far the torso may pitch backward, measured at the waist bone. In positive degrees.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Torso, meta = (UIMin = 0.0f, UIMax = 180.0f))
	float MaxPitchBackwardDegrees;

	// How far the torso may move side-to-side, rolling around the forward axis, in either direction. In positive degrees.
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Torso, meta = (UIMin = 0.0f, UIMax = 90.0f))
	//float MaxRollDegrees;

	// How far the torso may twist, around the character's spine direction, toward the left arm. Measured relative to the incoming animation pose, NOT the character forward direction. In degrees.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Torso, meta = (UIMin = 0.0f, UIMax = 180.0f))
	float MaxTwistDegreesLeft;

	// How far the torso may twist, around the character's spine direction, toward the right arm. Measured relative to the incoming animation pose, NOT the character forward direction. In degrees.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Torso, meta = (UIMin = 0.0f, UIMax = 180.0f))
	float MaxTwistDegreesRight;

	// The two arms will require different amounts of twist. If set to 1.0, the larger of the two twists is used;
	// if set to 0.0, the smaller twist is used. Increasing will cause the torso to twist more. You should usually leave this at 0.5.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Torso, meta = (UIMin = 0.0f, UIMax = 1.0f))
	float ArmTwistRatio;

	// Forward direction for this skeleton. Usually X axis, may sometimes be the Y axis.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Torso)
	EIKBoneAxis SkeletonForwardAxis;

	// Up direction for this skeleton. Should almost always be the Z axis.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Torso)
	EIKBoneAxis SkeletonUpAxis;

	// The waist bone indicates where the character will bend at the waist. Usually this is the first spine bone (the one closes to the pelvis)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Torso)
	FIKBone WaistBone;

	// How quickly the torso rotates (across all rotation axes). Increase to make torso rotation more responsive but snappier. This is a slerp value.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Torso)
	float TorsoRotationSlerpSpeed;

	// Where to place left arm effector, in world space
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings, meta = (PinShownByDefault))
	FTransform LeftArmWorldTarget;

	// Where to place right arm effector, in world space
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings, meta = (PinShownByDefault))
	FTransform RightArmWorldTarget;

	// How to handle rotation of the effector (the the hand). If set to No Change, the foot will maintain the same
	// rotation as before IK. If set to Maintain Local, it will maintain the same rotation relative to the parent as before IK. Copy Target Rotation is the same as No Change for now.	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Effector, meta = (PinHiddenByDefault))
	TEnumAsByte<EBoneRotationSource> EffectorRotationSource;

public:
	FAnimNode_HumanoidArmTorsoAdjust()
		: Mode(EHumanoidArmTorsoIKMode::IK_Human_ArmTorso_Disabled),
		bEnableDebugDraw(false),
		Precision(0.001f),
		MaxIterations(10),
		bEnable(true),
		// TorsoPivotSocketName(NAME_None),
		MaxShoulderDragDistance(50.0f),
		ShoulderDragStiffness(1.0f),
		MaxPitchForwardDegrees(60.0f),
		MaxPitchBackwardDegrees(10.0f),
		MaxTwistDegreesLeft(30.0f),
		MaxTwistDegreesRight(30.0f),		
		ArmTwistRatio(0.5f),
		SkeletonForwardAxis(EIKBoneAxis::IKBA_X),
		SkeletonUpAxis(EIKBoneAxis::IKBA_Z),\
		TorsoRotationSlerpSpeed(10.0f),
		LeftArmWorldTarget(FVector(0.0f, 0.0f, 0.0f)),
		RightArmWorldTarget(FVector(0.0f, 0.0f, 0.0f)),
		EffectorRotationSource(EBoneRotationSource::BRS_KeepComponentSpaceRotation),
		LastEffectorOffset(0.0f, 0.0f, 0.0f),
		LastRotationOffset(FQuat::Identity)
	{
		DeltaTime = 0.0f;
	}

	// 更新処理
	virtual void UpdateInternal(const FAnimationUpdateContext& Context) override
	{
		DeltaTime = Context.GetDeltaTime();
		//Super::UpdateInternal(Context);
	}

	// 影響を受けるボーンの新しいコンポーネント空間変換を評価
	virtual void EvaluateSkeletalControl_AnyThread(FComponentSpacePoseContext& Output, TArray<FBoneTransform>& OutBoneTransforms) override;

	// 骨格制御ベースを評価
	virtual void EvaluateComponentSpaceInternal(FComponentSpacePoseContext& Output) override
	{
		Super::EvaluateComponentSpaceInternal(Output);
	}

	// ノードの処理を行うかどうかの判定
	virtual bool IsValidToEvaluate(const USkeleton* Skeleton, const FBoneContainer& RequiredBones) override
	{
		if (LeftArm == nullptr || RightArm == nullptr)
		{
#if ENABLE_IK_DEBUG_VERBOSE
			UE_LOG(LogRTIK, Error, TEXT("An input wrapper object was null : %s"), *FString(__FUNCTION__));
#endif
			return false;
		}
		if (!LeftArm->IsValid(RequiredBones))
		{
#if ENABLE_IK_DEBUG_VERBOSE
			UE_LOG(LogRTIK, Warning, TEXT("Could not initialize LeftArm : %s"), *FString(__FUNCTION__));
#endif
			return false;
		}
		if (!RightArm->IsValid(RequiredBones))
		{
#if ENABLE_IK_DEBUG_VERBOSE
			UE_LOG(LogRTIK, Warning, TEXT("Could not initialize RightArm : %s"), *FString(__FUNCTION__));
#endif
			return false;
		}
		if (!WaistBone.IsValid(RequiredBones))
		{
#if ENABLE_IK_DEBUG_VERBOSE
			UE_LOG(LogRTIK, Warning, TEXT("Could not initialize Waist : %s"), *FString(__FUNCTION__));
#endif
			return false;
		}
		return true;
	}

	// ボーンの参照の初期化
	virtual void InitializeBoneReferences(const FBoneContainer& RequiredBones) override
	{
		if (LeftArm == nullptr || RightArm == nullptr)
		{
#if ENABLE_IK_DEBUG
			UE_LOG(LogRTIK, Error, TEXT("An input wrapper object was null : %s"), *FString(__FUNCTION__));
#endif
			return;
		}
		if (!LeftArm->InitBoneReferences(RequiredBones))
		{
#if ENABLE_IK_DEBUG
			UE_LOG(LogRTIK, Warning, TEXT("Could not initialize LeftArm : %s"), *FString(__FUNCTION__));
#endif
			return;
		}
		if (!RightArm->InitBoneReferences(RequiredBones))
		{
#if ENABLE_IK_DEBUG
			UE_LOG(LogRTIK, Warning, TEXT("Could not initialize RightArm : %s"), *FString(__FUNCTION__));
#endif
			return;
		}
		if (!WaistBone.Init(RequiredBones))
		{
#if ENABLE_IK_DEBUG
			UE_LOG(LogRTIK, Warning, TEXT("Could not initialize Waist : %s"), *FString(__FUNCTION__));
#endif
			return;
		}
	}

protected:
	float DeltaTime;
	FVector LastEffectorOffset;
	FQuat LastRotationOffset;
};
