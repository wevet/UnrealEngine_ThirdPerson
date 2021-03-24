
#include "AnimNode_HumanoidLegIK.h"
#include "RTIK.h"
#include "Components/SkeletalMeshComponent.h"
#include "TwoBoneIK.h"
#include "RangeLimitedFABRIK.h"
#include "Animation/AnimNodeBase.h"
#include "Animation/AnimInstanceProxy.h"
#include "IKFunctionLibrary.h"

#define ZERO 0.0f
#define ONE 1.0f

// ProfilerÇ…ìoò^Ç∑ÇÈ
DECLARE_CYCLE_STAT(TEXT("IK Humanoid Leg IK Eval"), STAT_HumanoidLegIK_Eval, STATGROUP_Anim);

// ë´ÇÃê›íuîªíËÇä«óùÇ∑ÇÈIK Class
void FAnimNode_HumanoidLegIK::EvaluateSkeletalControl_AnyThread(FComponentSpacePoseContext& Output, TArray<FBoneTransform>& OutBoneTransforms)
{
	SCOPE_CYCLE_COUNTER(STAT_HumanoidLegIK_Eval);

#if ENABLE_ANIM_DEBUG
	check(Output.AnimInstanceProxy->GetSkelMeshComponent());
#endif
	check(OutBoneTransforms.Num() == 0);

	USkeletalMeshComponent* SkelComp = Output.AnimInstanceProxy->GetSkelMeshComponent();
	const FMatrix ToCS = SkelComp->GetComponentToWorld().ToMatrixNoScale().Inverse();
	const FTransform HipCSTransform = UIKFunctionLibrary::GetBoneCSTransform(Output.Pose, Leg->GetChain().HipBone.BoneIndex);
	const FTransform KneeCSTransform = UIKFunctionLibrary::GetBoneCSTransform(Output.Pose, Leg->GetChain().ThighBone.BoneIndex);
	const FTransform FootCSTransform = UIKFunctionLibrary::GetBoneCSTransform(Output.Pose, Leg->GetChain().ShinBone.BoneIndex);
	const FVector HipCS = HipCSTransform.GetLocation();
	const FVector KneeCS = KneeCSTransform.GetLocation();
	const FVector FootCS = FootCSTransform.GetLocation();

	FVector FootTargetCS;
	FVector FloorCS;

	if (Mode == EHumanoidLegIKMode::IK_Human_Leg_Locomotion)
	{
		if (TraceData->GetTraceData().FootHitResult.GetActor() == nullptr && TraceData->GetTraceData().ToeHitResult.GetActor() == nullptr)
		{
			return;
		}

		FComponentSpacePoseContext BasePose(Output);
		BaseComponentPose.EvaluateComponentSpace(BasePose);
		const bool bWithinRotationLimit = Leg->GetChain().GetIKFloorPointCS(*SkelComp, TraceData->GetTraceData(), FloorCS);
		const FVector BaseRootCS = UIKFunctionLibrary::GetBoneCSLocation(BasePose.Pose, FCompactPoseBoneIndex(0));
		const FVector BaseFootCS = UIKFunctionLibrary::GetBoneCSLocation(BasePose.Pose, Leg->GetChain().ShinBone.BoneIndex);
		const float FootHeightAboveRoot = BaseFootCS.Z - BaseRootCS.Z;
		const float MinimumFootHeight = FloorCS.Z + FootHeightAboveRoot;
		FootTargetCS = (FootCS.Z < MinimumFootHeight) ? FVector(FootCS.X, FootCS.Y, FloorCS.Z + FootHeightAboveRoot) : FootCS;
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
			Leg->GetChain().HipBone.GetConstraint(),
			Leg->GetChain().ThighBone.GetConstraint(),
			Leg->GetChain().ShinBone.GetConstraint()
		});

		FRangeLimitedFABRIK::SolveRangeLimitedFABRIK(
			DestCSTransforms,
			SourceCSTransforms,
			Constraints,
			FootTargetCS,
			ZERO,
			ONE,
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
			FVector(1.0f, 0.0f, 0.0f),
			FootTargetCS,
			false,
			ONE,
			ONE);
	}

	OutBoneTransforms.Add(FBoneTransform(Leg->GetChain().HipBone.BoneIndex, DestCSTransforms[0]));
	OutBoneTransforms.Add(FBoneTransform(Leg->GetChain().ThighBone.BoneIndex, DestCSTransforms[1]));
	OutBoneTransforms.Add(FBoneTransform(Leg->GetChain().ShinBone.BoneIndex, DestCSTransforms[2]));

#if WITH_EDITOR
	if (bEnableDebugDraw)
	{
		UWorld* World = SkelComp->GetWorld();
		FMatrix ToWorld = SkelComp->GetComponentToWorld().ToMatrixNoScale();
		FVector EffectorWorld = ToWorld.TransformPosition(FootTargetCS);

		UIKFunctionLibrary::DrawSphere(World, EffectorWorld, FColor(255, 0, 255));
		UIKFunctionLibrary::DrawSphere(World, ToWorld.TransformPosition(FloorCS), FColor(255, 0, 0));
		UIKFunctionLibrary::DrawSphere(World, TraceData->GetTraceData().FootHitResult.ImpactPoint, FColor(255, 255, 0), 10.0f);
		UIKFunctionLibrary::DrawSphere(World, TraceData->GetTraceData().ToeHitResult.ImpactPoint, FColor(255, 255, 0), 10.0f);

		// Leg before IK, in yellow:
		UIKFunctionLibrary::DrawLine(World, ToWorld.TransformPosition(HipCSTransform.GetLocation()), ToWorld.TransformPosition(KneeCSTransform.GetLocation()), FColor(255, 255, 0));
		UIKFunctionLibrary::DrawLine(World, ToWorld.TransformPosition(KneeCSTransform.GetLocation()), ToWorld.TransformPosition(FootCSTransform.GetLocation()), FColor(255, 255, 0));

		// Leg after IK, in cyan
		UIKFunctionLibrary::DrawLine(World, ToWorld.TransformPosition(DestCSTransforms[0].GetLocation()), ToWorld.TransformPosition(DestCSTransforms[1].GetLocation()), FColor(0, 255, 255));
		UIKFunctionLibrary::DrawLine(World, ToWorld.TransformPosition(DestCSTransforms[1].GetLocation()), ToWorld.TransformPosition(DestCSTransforms[2].GetLocation()), FColor(0, 255, 255));
	}
#endif
}

