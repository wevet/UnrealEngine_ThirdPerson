
#include "AnimNode_RangeLimitedFabrik.h"
#include "RTIK.h"
#include "AnimationRuntime.h"
#include "Animation/AnimInstanceProxy.h"
#include "Components/SkeletalMeshComponent.h"
#include "RangeLimitedFABRIK.h"


// ProfilerÇ…ìoò^Ç∑ÇÈ
DECLARE_CYCLE_STAT(TEXT("IK Range Limited FABRIK"), STAT_RangeLimitedFabrik_Eval, STATGROUP_Anim);

// FABRIK IK(Forward And Backward Reaching inverse kinematics)ÇÕÅA
// ç™å≥Ç∆êÊí[Çåå›Ç…äÓèÄÇ∆ÇµÇƒåJÇËï‘ÇµÇ»Ç™ÇÁä÷êﬂäpìxÇí≤êÆ
void FAnimNode_RangeLimitedFabrik::EvaluateSkeletalControl_AnyThread(FComponentSpacePoseContext& Output, TArray<FBoneTransform>& OutBoneTransforms)
{
	SCOPE_CYCLE_COUNTER(STAT_RangeLimitedFabrik_Eval);

	const FBoneContainer& BoneContainer = Output.Pose.GetPose().GetBoneContainer();

	FTransform OutEffectorTransform = EffectorTransform;
	FAnimationRuntime::ConvertBoneSpaceTransformToCS(
		Output.AnimInstanceProxy->GetComponentTransform(),
		Output.Pose, 
		OutEffectorTransform, 
		EffectorTransformBone.GetCompactPoseIndex(BoneContainer),
		EffectorTransformSpace);

	const FVector CSEffectorLocation = OutEffectorTransform.GetLocation();

#if WITH_EDITOR
	CachedEffectorCSTransform = OutEffectorTransform;
#endif	
	check(OutBoneTransforms.Num() == 0);


	const int32 NumChainLinks = IKChain->GetChain().Num();
	if (NumChainLinks < 2)
	{
		return;
	}

	// Maximum length of skeleton segment at full extension
	float MaximumReach = 0;

	// Gather bone transforms and constraints
	TArray<FTransform> SourceCSTransforms;
	TArray<FIKBoneConstraint*> Constraints;
	SourceCSTransforms.Reserve(NumChainLinks);
	Constraints.Reserve(NumChainLinks);
	for (int32 i = 0; i < NumChainLinks; ++i)
	{
		SourceCSTransforms.Add(Output.Pose.GetComponentSpaceTransform(IKChain->GetChain()[i].BoneIndex));
		Constraints.Add(IKChain->GetChain()[i].GetConstraint());
	}


	TArray<FTransform> DestCSTransforms;
	ACharacter* Character = Cast<ACharacter>(Output.AnimInstanceProxy->GetSkelMeshComponent()->GetOwner());
	bool bBoneLocationUpdated = false;
	if (SolverMode == ERangeLimitedFABRIKSolverMode::RLF_Normal)
	{
		bBoneLocationUpdated = FRangeLimitedFABRIK::SolveRangeLimitedFABRIK(
			DestCSTransforms,
			SourceCSTransforms,
			Constraints,
			OutEffectorTransform.GetLocation(),
			MaxRootDragDistance,
			RootDragStiffness,
			Precision,
			MaxIterations,
			Character);
	}
	else if (SolverMode == ERangeLimitedFABRIKSolverMode::RLF_ClosedLoop)
	{
		bBoneLocationUpdated = FRangeLimitedFABRIK::SolveClosedLoopFABRIK(
			DestCSTransforms,
			SourceCSTransforms,
			Constraints,
			OutEffectorTransform.GetLocation(),
			MaxRootDragDistance,
			RootDragStiffness,
			Precision,
			MaxIterations,
			Character);
	}


	// Special handling for tip bone's rotation.
	const int32 TipBoneIndex = (NumChainLinks - 1);
	switch (EffectorRotationSource)
	{
		case BRS_KeepLocalSpaceRotation:
		if (NumChainLinks > 1)
		{
			DestCSTransforms[TipBoneIndex] = Output.Pose.GetLocalSpaceTransform(IKChain->GetChain()[TipBoneIndex].BoneIndex) * DestCSTransforms[TipBoneIndex - 1];
		}
		break;
		case BRS_CopyFromTarget:
		{
			DestCSTransforms[TipBoneIndex].SetRotation(OutEffectorTransform.GetRotation());
		}
		break;
		case BRS_KeepComponentSpaceRotation:
		{
			// Don't change the orientation at all
		}
		break;
		default:
		{
			//
		}
		break;
	}

	if (bBoneLocationUpdated)
	{
		OutBoneTransforms.Reserve(NumChainLinks);
		for (int32 i = 0; i < NumChainLinks; ++i)
		{
			OutBoneTransforms.Add(FBoneTransform(IKChain->GetChain()[i].BoneIndex, DestCSTransforms[i]));
		}
	}

#if WITH_EDITOR
	if (bEnableDebugDraw)
	{
		USkeletalMeshComponent* SkelComp = Output.AnimInstanceProxy->GetSkelMeshComponent();
		UWorld* World = SkelComp->GetWorld();
		const FMatrix ConvertMatrix = SkelComp->GetComponentToWorld().ToMatrixNoScale();

		if (SolverMode == ERangeLimitedFABRIKSolverMode::RLF_Normal)
		{
			for (int32 i = 0; i < NumChainLinks - 1; ++i)
			{
				const FVector ParentLoc = ConvertMatrix.TransformPosition(SourceCSTransforms[i].GetLocation());
				const FVector ChildLoc = ConvertMatrix.TransformPosition(SourceCSTransforms[i + 1].GetLocation());
				UIKFunctionLibrary::DrawLine(World, ParentLoc, ChildLoc, FColor(255, 255, 0));
				UIKFunctionLibrary::DrawSphere(World, ChildLoc, FColor(255, 255, 0), 3.0f);
			}

			if (MaxRootDragDistance > KINDA_SMALL_NUMBER)
			{
				UIKFunctionLibrary::DrawSphere(World, ConvertMatrix.TransformPosition(SourceCSTransforms[0].GetLocation()), FColor(255, 0, 0), MaxRootDragDistance, 12, -1.0f, 0.5f);
			}

			for (int32 i = 0; i < NumChainLinks - 1; ++i)
			{
				const FVector ParentLoc = ConvertMatrix.TransformPosition(DestCSTransforms[i].GetLocation());
				const FVector ChildLoc = ConvertMatrix.TransformPosition(DestCSTransforms[i + 1].GetLocation());
				UIKFunctionLibrary::DrawLine(World, ParentLoc, ChildLoc, FColor(0, 255, 255));
				UIKFunctionLibrary::DrawSphere(World, ChildLoc, FColor(0, 255, 255), 3.0f);
			}
		}
		else if (SolverMode == ERangeLimitedFABRIKSolverMode::RLF_ClosedLoop)
		{
			for (int32 i = 0; i < NumChainLinks - 1; ++i)
			{
				const FVector ParentLoc = ConvertMatrix.TransformPosition(SourceCSTransforms[i].GetLocation());
				const FVector ChildLoc = ConvertMatrix.TransformPosition(SourceCSTransforms[i + 1].GetLocation());
				UIKFunctionLibrary::DrawLine(World, ParentLoc, ChildLoc, FColor(255, 255, 0));
				UIKFunctionLibrary::DrawSphere(World, ChildLoc, FColor(255, 255, 0), 3.0f);
			}
			UIKFunctionLibrary::DrawLine(World, ConvertMatrix.TransformPosition(SourceCSTransforms[0].GetLocation()), ConvertMatrix.TransformPosition(SourceCSTransforms[NumChainLinks - 1].GetLocation()), FColor(255, 255, 0));

			if (MaxRootDragDistance > KINDA_SMALL_NUMBER)
			{
				UIKFunctionLibrary::DrawSphere(World, ConvertMatrix.TransformPosition(SourceCSTransforms[0].GetLocation()), FColor(255, 0, 0), MaxRootDragDistance, 12, -1.0f, 0.5f);
			}

			for (int32 i = 0; i < NumChainLinks - 1; ++i)
			{
				const FVector ParentLoc = ConvertMatrix.TransformPosition(DestCSTransforms[i].GetLocation());
				const FVector ChildLoc = ConvertMatrix.TransformPosition(DestCSTransforms[i + 1].GetLocation());
				UIKFunctionLibrary::DrawLine(World, ParentLoc, ChildLoc, FColor(0, 255, 255));
				UIKFunctionLibrary::DrawSphere(World, ChildLoc, FColor(0, 255, 255), 3.0f);
			}
			UIKFunctionLibrary::DrawLine(World, ConvertMatrix.TransformPosition(DestCSTransforms[0].GetLocation()), ConvertMatrix.TransformPosition(DestCSTransforms[NumChainLinks - 1].GetLocation()), FColor(0, 255, 255));
		}
	}
#endif
}
