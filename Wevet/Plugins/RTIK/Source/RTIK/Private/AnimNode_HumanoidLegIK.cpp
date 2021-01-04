
#include "AnimNode_HumanoidLegIK.h"
#include "RTIK.h"
#include "Components/SkeletalMeshComponent.h"
#include "TwoBoneIK.h"
#include "RangeLimitedFABRIK.h"
#include "Animation/AnimNodeBase.h"
#include "Animation/AnimInstanceProxy.h"
#include "AnimUtil.h"

#if WITH_EDITOR
#include "DebugDrawUtil.h"
#endif

DECLARE_CYCLE_STAT(TEXT("IK Humanoid Leg IK Eval"), STAT_HumanoidLegIK_Eval, STATGROUP_Anim);


void FAnimNode_HumanoidLegIK::EvaluateSkeletalControl_AnyThread(FComponentSpacePoseContext& Output, TArray<FBoneTransform>& OutBoneTransforms)
{
	SCOPE_CYCLE_COUNTER(STAT_HumanoidLegIK_Eval);

#if ENABLE_ANIM_DEBUG
	check(Output.AnimInstanceProxy->GetSkelMeshComponent());
#endif
	check(OutBoneTransforms.Num() == 0);

	// Input pin pointers are checked in IsValid -- don't need to check here
	USkeletalMeshComponent* SkelComp = Output.AnimInstanceProxy->GetSkelMeshComponent();

	const FMatrix ToCS = SkelComp->GetComponentToWorld().ToMatrixNoScale().Inverse();
	const FTransform HipCSTransform = FAnimUtil::GetBoneCSTransform(*SkelComp, Output.Pose, Leg->Chain.HipBone.BoneIndex);
	const FTransform KneeCSTransform = FAnimUtil::GetBoneCSTransform(*SkelComp, Output.Pose, Leg->Chain.ThighBone.BoneIndex);
	const FTransform FootCSTransform = FAnimUtil::GetBoneCSTransform(*SkelComp, Output.Pose, Leg->Chain.ShinBone.BoneIndex);
	const FVector HipCS = HipCSTransform.GetLocation();
	const FVector KneeCS = KneeCSTransform.GetLocation();
	const FVector FootCS = FootCSTransform.GetLocation();

	FVector FootTargetCS;
	FVector FloorCS;

	if (Mode == EHumanoidLegIKMode::IK_Human_Leg_Locomotion)
	{
		if (TraceData->GetTraceData().FootHitResult.GetActor() == nullptr && TraceData->GetTraceData().ToeHitResult.GetActor() == nullptr)
		{
#if ENABLE_IK_DEBUG_VERBOSE
			UE_LOG(LogRTIK, Warning, TEXT("Leg IK trace did not hit a valid actor : %s"), *FString(__FUNCTION__));
#endif
			return;
		}

		FComponentSpacePoseContext BasePose(Output);
		BaseComponentPose.EvaluateComponentSpace(BasePose);

		const bool bWithinRotationLimit = Leg->Chain.GetIKFloorPointCS(*SkelComp, TraceData->GetTraceData(), FloorCS);
		const FVector BaseRootCS = FAnimUtil::GetBoneCSLocation(*SkelComp, BasePose.Pose, FCompactPoseBoneIndex(0));
		const FVector BaseFootCS = FAnimUtil::GetBoneCSLocation(*SkelComp, BasePose.Pose, Leg->Chain.ShinBone.BoneIndex);

		const float FootHeightAboveRoot = BaseFootCS.Z - BaseRootCS.Z;
		const float MinimumFootHeight = FloorCS.Z + FootHeightAboveRoot;

		// Don't move foot unless the foot is below the target height
		if (FootCS.Z < MinimumFootHeight)
		{
			//Again, foot radius is now ignored. Don't need it 
			//FootTargetCS = FVector(FootCS.X, FootCS.Y, FloorCS.Z + FootHeightAboveRoot + Leg->Chain.FootRadius);
			FootTargetCS = FVector(FootCS.X, FootCS.Y, FloorCS.Z + FootHeightAboveRoot);
		}
		else
		{
			FootTargetCS = FootCS;
		}

	}
	else
	{
		FootTargetCS = ToCS.TransformPosition(FootTargetWorld.GetLocation());
	}

	if (bEffectorMovesInstantly)
	{
		LastEffectorOffset = FVector(0.0f, 0.0f, 0.0f);
	}
	else
	{
		const FVector OffsetFootPos = FootCS + LastEffectorOffset;
		FVector RequiredDelta = FootTargetCS - OffsetFootPos;

		if (RequiredDelta.Size() > EffectorVelocity * DeltaTime)
		{
			RequiredDelta = RequiredDelta.GetClampedToMaxSize(EffectorVelocity * DeltaTime);
		}

		FootTargetCS = OffsetFootPos + RequiredDelta;
		LastEffectorOffset = LastEffectorOffset + RequiredDelta;
	}

	TArray<FTransform> DestCSTransforms;
	if (Solver == EHumanoidLegIKSolver::IK_Human_Leg_Solver_FABRIK)
	{
		TArray<FTransform> SourceCSTransforms(
		{
			HipCSTransform,
			KneeCSTransform,
			FootCSTransform
		});

		TArray<FIKBoneConstraint*> Constraints(
		{
			Leg->Chain.HipBone.GetConstraint(),
			Leg->Chain.ThighBone.GetConstraint(),
			Leg->Chain.ShinBone.GetConstraint()
		});

		FRangeLimitedFABRIK::SolveRangeLimitedFABRIK(
			SourceCSTransforms,
			Constraints,
			FootTargetCS,
			DestCSTransforms,
			0.0f,
			1.0f,
			Precision,
			MaxIterations,
			Cast<ACharacter>(SkelComp->GetOwner()));

	}
	else if (Solver == EHumanoidLegIKSolver::IK_Human_Leg_Solver_TwoBone)
	{
		DestCSTransforms.Reserve(3);
		DestCSTransforms.Add(HipCSTransform);
		DestCSTransforms.Add(KneeCSTransform);
		DestCSTransforms.Add(FootCSTransform);

		AnimationCore::SolveTwoBoneIK(
			DestCSTransforms[0],
			DestCSTransforms[1],
			DestCSTransforms[2],
			FVector(1.0f, 0.0f, 0.0f), // Use forward direction, knee correction node will fix it
			FootTargetCS,
			false,
			1.0f,
			1.0f);
	}

	OutBoneTransforms.Add(FBoneTransform(Leg->Chain.HipBone.BoneIndex, DestCSTransforms[0]));
	OutBoneTransforms.Add(FBoneTransform(Leg->Chain.ThighBone.BoneIndex, DestCSTransforms[1]));
	OutBoneTransforms.Add(FBoneTransform(Leg->Chain.ShinBone.BoneIndex, DestCSTransforms[2]));

#if WITH_EDITOR
	if (bEnableDebugDraw)
	{
		UWorld* World = SkelComp->GetWorld();
		FMatrix ToWorld = SkelComp->GetComponentToWorld().ToMatrixNoScale();
		FVector EffectorWorld = ToWorld.TransformPosition(FootTargetCS);

		FDebugDrawUtil::DrawSphere(World, EffectorWorld, FColor(255, 0, 255));
		FDebugDrawUtil::DrawSphere(World, ToWorld.TransformPosition(FloorCS), FColor(255, 0, 0));
		FDebugDrawUtil::DrawSphere(World, TraceData->GetTraceData().FootHitResult.ImpactPoint, FColor(255, 255, 0), 10.0f);
		FDebugDrawUtil::DrawSphere(World, TraceData->GetTraceData().ToeHitResult.ImpactPoint, FColor(255, 255, 0), 10.0f);

		// Leg before IK, in yellow:
		FDebugDrawUtil::DrawLine(
			World,
			ToWorld.TransformPosition(HipCSTransform.GetLocation()),
			ToWorld.TransformPosition(KneeCSTransform.GetLocation()),
			FColor(255, 255, 0));

		FDebugDrawUtil::DrawLine(
			World,
			ToWorld.TransformPosition(KneeCSTransform.GetLocation()),
			ToWorld.TransformPosition(FootCSTransform.GetLocation()),
			FColor(255, 255, 0));

		// Leg after IK, in cyan
		FDebugDrawUtil::DrawLine(
			World,
			ToWorld.TransformPosition(DestCSTransforms[0].GetLocation()),
			ToWorld.TransformPosition(DestCSTransforms[1].GetLocation()),
			FColor(0, 255, 255));

		FDebugDrawUtil::DrawLine(
			World,
			ToWorld.TransformPosition(DestCSTransforms[1].GetLocation()),
			ToWorld.TransformPosition(DestCSTransforms[2].GetLocation()),
			FColor(0, 255, 255));
	}
#endif
}

