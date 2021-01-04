
#include "AnimNode_HumanoidArmTorsoAdjust.h"
#include "RTIK.h"
#include "Components/SkeletalMeshComponent.h"
#include "Animation/AnimInstanceProxy.h"
#include "AnimationRuntime.h"
#include "Constraints.h"
#include "RangeLimitedFABRIK.h"
#include "AnimUtil.h"

#if WITH_EDITOR
#include "DebugDrawUtil.h"
#endif

DECLARE_CYCLE_STAT(TEXT("IK Humanoid Arm Torso Adjust"), STAT_HumanoidArmTorsoAdjust_Eval, STATGROUP_Anim);


void FAnimNode_HumanoidArmTorsoAdjust::EvaluateSkeletalControl_AnyThread(FComponentSpacePoseContext& Output, TArray<FBoneTransform>& OutBoneTransforms)
{
	SCOPE_CYCLE_COUNTER(STAT_HumanoidArmTorsoAdjust_Eval);

#if ENABLE_ANIM_DEBUG
	check(Output.AnimInstanceProxy->GetSkelMeshComponent());
#endif
	check(OutBoneTransforms.Num() == 0);

	if (!LeftArm || !RightArm)
	{
		return;
	}

	USkeletalMeshComponent* SkelComp = Output.AnimInstanceProxy->GetSkelMeshComponent();
	const FMatrix ToCS = SkelComp->GetComponentToWorld().ToMatrixNoScale().Inverse();
	const int32 NumBonesLeft = LeftArm->Chain.Num();
	const int32 NumBonesRight = RightArm->Chain.Num();
	if (NumBonesLeft < 1 || NumBonesRight < 1)
	{
		return;
	}

	// Get skeleton axes
	const FVector ForwardAxis = FIKUtil::IKBoneAxisToVector(SkeletonForwardAxis);
	const FVector UpAxis = FIKUtil::IKBoneAxisToVector(SkeletonUpAxis);
	const FVector LeftAxis = FVector::CrossProduct(ForwardAxis, UpAxis);
	const FVector RightAxis = -1 * LeftAxis;

	FTransform WaistCS = Output.Pose.GetComponentSpaceTransform(WaistBone.BoneIndex);

#if ENABLE_IK_DEBUG
	if (!LeftAxis.IsNormalized())
	{
		UE_LOG(LogRTIK, Warning, TEXT("Could not evaluate Humanoid Arm Torso Adjustment - Skeleton Forward Axis and Skeleton Up Axis were not orthogonal : %s"), *FString(__FUNCTION__));
		return;
	}
#endif

	TArray<FTransform> CSTransformsLeft;
	CSTransformsLeft.Reserve(NumBonesLeft);
	for (FIKBone& Bone : LeftArm->Chain.BonesRootToEffector)
	{
		CSTransformsLeft.Add(Output.Pose.GetComponentSpaceTransform(Bone.BoneIndex));
	}
	CSTransformsLeft.Shrink();

	TArray<FTransform> CSTransformsRight;
	CSTransformsLeft.Reserve(NumBonesRight);
	for (FIKBone& Bone : RightArm->Chain.BonesRootToEffector)
	{
		CSTransformsRight.Add(Output.Pose.GetComponentSpaceTransform(Bone.BoneIndex));
	}
	CSTransformsRight.Shrink();

	// Setup constraints
	TArray<FIKBoneConstraint*> ConstraintsLeft;
	ConstraintsLeft.Reserve(NumBonesLeft);
	for (FIKBone& Bone : LeftArm->Chain.BonesRootToEffector)
	{
		ConstraintsLeft.Add(Bone.GetConstraint());
	}
	ConstraintsLeft.Shrink();

	TArray<FIKBoneConstraint*> ConstraintsRight;
	ConstraintsRight.Reserve(NumBonesLeft);
	for (FIKBone& Bone : RightArm->Chain.BonesRootToEffector)
	{
		ConstraintsRight.Add(Bone.GetConstraint());
	}
	ConstraintsRight.Shrink();

	// First pass: IK each arm, allowing shoulders to drag
	TArray<FTransform> PostIKTransformsLeft;
	if (Mode == EHumanoidArmTorsoIKMode::IK_Human_ArmTorso_BothArms || Mode == EHumanoidArmTorsoIKMode::IK_Human_ArmTorso_LeftArmOnly)
	{
		FVector LeftTargetCS = ToCS.TransformPosition(LeftArmWorldTarget.GetLocation());
		FRangeLimitedFABRIK::SolveRangeLimitedFABRIK(
			CSTransformsLeft,
			ConstraintsLeft,
			LeftTargetCS,
			PostIKTransformsLeft,
			MaxShoulderDragDistance,
			ShoulderDragStiffness,
			Precision,
			MaxIterations,
			Cast<ACharacter>(SkelComp->GetOwner()));
	}
	else
	{
		for (FTransform& Transform : CSTransformsLeft)
		{
			PostIKTransformsLeft.Add(Transform);
		}
	}

	TArray<FTransform> PostIKTransformsRight;
	if (Mode == EHumanoidArmTorsoIKMode::IK_Human_ArmTorso_BothArms || Mode == EHumanoidArmTorsoIKMode::IK_Human_ArmTorso_RightArmOnly)
	{
		FVector RightTargetCS = ToCS.TransformPosition(RightArmWorldTarget.GetLocation());
		FRangeLimitedFABRIK::SolveRangeLimitedFABRIK(
			CSTransformsRight,
			ConstraintsRight,
			RightTargetCS,
			PostIKTransformsRight,
			MaxShoulderDragDistance,
			ShoulderDragStiffness,
			Precision,
			MaxIterations,
			Cast<ACharacter>(SkelComp->GetOwner()));
	}
	else
	{
		for (FTransform& Transform : CSTransformsRight)
		{
			PostIKTransformsRight.Add(Transform);
		}
	}

	FTransform WaistCSPostIK = WaistCS;

	// 'Neck' is the midpoint beween the shoulders
	const FVector ShoulderLeftPreIK = CSTransformsLeft[0].GetLocation() - WaistCS.GetLocation();
	const FVector ShoulderRightPreIK = CSTransformsRight[0].GetLocation() - WaistCS.GetLocation();
	const FVector NeckPreIK = (ShoulderLeftPreIK + ShoulderRightPreIK) / 2;
	const FVector SpineDirection = NeckPreIK.GetUnsafeNormal();

	const FVector ShoulderLeftPostIK = PostIKTransformsLeft[0].GetLocation() - WaistCSPostIK.GetLocation();
	const FVector ShoulderRightPostIK = PostIKTransformsRight[0].GetLocation() - WaistCSPostIK.GetLocation();
	const FVector NeckPostIK = (ShoulderLeftPostIK + ShoulderRightPostIK) / 2;
	const FVector SpineDirectionPost = NeckPostIK.GetUnsafeNormal();

	const FVector ShoulderLeftPostIKDir = (ShoulderLeftPostIK - NeckPostIK).GetUnsafeNormal();
	const FVector ShoulderRightPostIKDir = (ShoulderRightPostIK - NeckPostIK).GetUnsafeNormal();
	const FVector ShoulderLeftPreIKDir = (ShoulderLeftPreIK - NeckPreIK).GetUnsafeNormal();
	const FVector ShoulderRightPreIKDir = (ShoulderRightPreIK - NeckPreIK).GetUnsafeNormal();

	// Find the twist angle by blending the small rotation and the large one as specified
	FVector LeftTwistAxis = FVector::CrossProduct(ShoulderLeftPreIKDir, ShoulderLeftPostIKDir);
	float LeftTwistRad = 0.0f;
	if (LeftTwistAxis.Normalize())
	{
		LeftTwistRad = (FVector::DotProduct(LeftTwistAxis, SpineDirection) > 0.0f) ?
			FMath::Acos(FVector::DotProduct(ShoulderLeftPreIKDir, ShoulderLeftPostIKDir)) :
			-1 * FMath::Acos(FVector::DotProduct(ShoulderLeftPreIKDir, ShoulderLeftPostIKDir));
	}

	FVector RightTwistAxis = FVector::CrossProduct(ShoulderRightPreIKDir, ShoulderRightPostIKDir);
	float RightTwistRad = 0.0f;
	if (RightTwistAxis.Normalize())
	{
		RightTwistRad = (FVector::DotProduct(RightTwistAxis, SpineDirection) > 0.0f) ?
			FMath::Acos(FVector::DotProduct(ShoulderRightPreIKDir, ShoulderRightPostIKDir)) :
			-1 * FMath::Acos(FVector::DotProduct(ShoulderRightPreIKDir, ShoulderRightPostIKDir));
	}

	float SmallRad;
	float LargeRad;
	if (FMath::Abs(LeftTwistRad) > FMath::Abs(RightTwistRad))
	{
		SmallRad = RightTwistRad;
		LargeRad = LeftTwistRad;
	}
	else
	{
		SmallRad = LeftTwistRad;
		LargeRad = RightTwistRad;
	}

	float TwistRad;
	if (Mode == EHumanoidArmTorsoIKMode::IK_Human_ArmTorso_BothArms)
	{
		// If IKing both arms, blend the twists
		TwistRad = FMath::Lerp(SmallRad, LargeRad, ArmTwistRatio);
	}
	else
	{
		// Otherwise, use the large twist; the other arm isn't iking and shouldn't contribute
		TwistRad = LargeRad;
	}

	float TwistDeg = FMath::RadiansToDegrees(TwistRad);
	TwistDeg = FMath::Clamp(TwistDeg, -MaxTwistDegreesLeft, MaxTwistDegreesRight);
	const FQuat TwistRotation(SpineDirection, FMath::DegreesToRadians(TwistDeg));

	// Prepare pitch (bend forward / backward) rotation
	const FVector SpinePitchPreIK = FVector::VectorPlaneProject(SpineDirection, LeftAxis);
	const FVector SpinePitchPostIK = FVector::VectorPlaneProject(SpineDirectionPost, LeftAxis);

	FVector PitchPositiveDirection = FVector::CrossProduct(SpinePitchPreIK, SpinePitchPostIK);
	float PitchRad = 0.0f;
	if (PitchPositiveDirection.Normalize())
	{
		PitchRad = (FVector::DotProduct(PitchPositiveDirection, RightAxis) >= 0.0f) ?
			FMath::Acos(FVector::DotProduct(SpinePitchPreIK, SpinePitchPostIK)) :
			-1 * FMath::Acos(FVector::DotProduct(SpinePitchPreIK, SpinePitchPostIK));
	}

	PitchRad = FMath::DegreesToRadians(FMath::Clamp(FMath::RadiansToDegrees(PitchRad), -MaxPitchBackwardDegrees, MaxPitchForwardDegrees));
	const FQuat PitchRotation(RightAxis, PitchRad);
	const FQuat TargetOffset = (PitchRotation * TwistRotation);
	LastRotationOffset = FQuat::Slerp(LastRotationOffset, TargetOffset, FMath::Clamp(TorsoRotationSlerpSpeed * DeltaTime, 0.0f, 1.0f));

	WaistCS.SetRotation((LastRotationOffset * WaistCS.GetRotation()).GetNormalized());
	OutBoneTransforms.Add(FBoneTransform(WaistBone.BoneIndex, WaistCS));

#if WITH_EDITOR
	if (bEnableDebugDraw)
	{
		UWorld* World = SkelComp->GetWorld();
		FMatrix ToWorld = SkelComp->GetComponentToWorld().ToMatrixNoScale();
		FVector WaistLocWorld = ToWorld.TransformPosition(WaistCS.GetLocation());
		FVector WaistLocWorldPostIK = ToWorld.TransformPosition(WaistCSPostIK.GetLocation());
		FVector ParentLoc;
		FVector ChildLoc;

		// Draw chain before adjustment, in yellow
		for (int32 i = 0; i < NumBonesLeft - 1; ++i)
		{
			// Draw each arm
			ParentLoc = ToWorld.TransformPosition(CSTransformsLeft[i].GetLocation());
			ChildLoc = ToWorld.TransformPosition(CSTransformsLeft[i + 1].GetLocation());
			FDebugDrawUtil::DrawLine(World, ParentLoc, ChildLoc, FColor(255, 255, 0));
			FDebugDrawUtil::DrawSphere(World, ChildLoc, FColor(255, 255, 0), 3.0f);

			ParentLoc = ToWorld.TransformPosition(CSTransformsRight[i].GetLocation());
			ChildLoc = ToWorld.TransformPosition(CSTransformsRight[i + 1].GetLocation());
			FDebugDrawUtil::DrawLine(World, ParentLoc, ChildLoc, FColor(255, 255, 0));
			FDebugDrawUtil::DrawSphere(World, ChildLoc, FColor(255, 255, 0), 3.0f);
		}
		// Draw torso triangle
		FDebugDrawUtil::DrawLine(World, WaistLocWorld, ToWorld.TransformPosition(CSTransformsLeft[0].GetLocation()), FColor(255, 255, 0));
		FDebugDrawUtil::DrawLine(World, WaistLocWorld, ToWorld.TransformPosition(CSTransformsRight[0].GetLocation()),FColor(255, 255, 0));
		FDebugDrawUtil::DrawLine(World, ToWorld.TransformPosition(CSTransformsLeft[0].GetLocation()), ToWorld.TransformPosition(CSTransformsRight[0].GetLocation()), FColor(255, 255, 0));
		FDebugDrawUtil::DrawSphere(World, WaistLocWorld, FColor(255, 255, 0), 3.0f);

		FVector NeckPointPre = ToWorld.TransformPosition((CSTransformsLeft[0].GetLocation() + CSTransformsRight[0].GetLocation()) / 2);
		FDebugDrawUtil::DrawLine(World, WaistLocWorld, NeckPointPre, FColor(255, 255, 0));
		FDebugDrawUtil::DrawSphere(World, NeckPointPre, FColor(255, 255, 0), 3.0f);

		// Draw chain after adjustment, in cyan
		for (int32 i = 0; i < NumBonesLeft - 1; ++i)
		{
			// Draw each arm
			ParentLoc = ToWorld.TransformPosition(PostIKTransformsLeft[i].GetLocation());
			ChildLoc = ToWorld.TransformPosition(PostIKTransformsLeft[i + 1].GetLocation());
			FDebugDrawUtil::DrawLine(World, ParentLoc, ChildLoc, FColor(0, 255, 255));
			FDebugDrawUtil::DrawSphere(World, ChildLoc, FColor(0, 255, 255), 3.0f);

			ParentLoc = ToWorld.TransformPosition(PostIKTransformsRight[i].GetLocation());
			ChildLoc = ToWorld.TransformPosition(PostIKTransformsRight[i + 1].GetLocation());
			FDebugDrawUtil::DrawLine(World, ParentLoc, ChildLoc, FColor(0, 255, 255));
			FDebugDrawUtil::DrawSphere(World, ChildLoc, FColor(0, 255, 255), 3.0f);
		}
		// Draw torso triangle
		FDebugDrawUtil::DrawLine(World, WaistLocWorldPostIK, ToWorld.TransformPosition(PostIKTransformsLeft[0].GetLocation()), FColor(0, 255, 255));
		FDebugDrawUtil::DrawLine(World, WaistLocWorldPostIK, ToWorld.TransformPosition(PostIKTransformsRight[0].GetLocation()), FColor(0, 255, 255));
		FDebugDrawUtil::DrawLine(World, ToWorld.TransformPosition(PostIKTransformsLeft[0].GetLocation()), ToWorld.TransformPosition(PostIKTransformsRight[0].GetLocation()), FColor(0, 255, 255));
		FDebugDrawUtil::DrawSphere(World, WaistLocWorldPostIK, FColor(0, 255, 255), 3.0f);

		FVector NeckPointPost = ToWorld.TransformPosition((PostIKTransformsLeft[0].GetLocation() + PostIKTransformsRight[0].GetLocation()) / 2);
		FDebugDrawUtil::DrawLine(World, WaistLocWorldPostIK, NeckPointPost, FColor(0, 255, 255));
		FDebugDrawUtil::DrawSphere(World, NeckPointPost, FColor(0, 255, 255), 3.0f);

		// Draw skeleton axes
		FVector Base = ToWorld.GetOrigin();
		FDebugDrawUtil::DrawVector(World, Base, ToWorld.TransformVector(ForwardAxis), FColor(255, 0, 0));
		FDebugDrawUtil::DrawVector(World, Base, ToWorld.TransformVector(LeftAxis), FColor(0, 255, 0));
		FDebugDrawUtil::DrawVector(World, Base, ToWorld.TransformVector(UpAxis), FColor(0, 0, 255));

	}
#endif
}

