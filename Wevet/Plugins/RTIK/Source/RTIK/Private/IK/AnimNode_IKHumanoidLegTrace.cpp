
#include "AnimNode_IKHumanoidLegTrace.h"
#include "RTIK.h"
#include "Components/SkeletalMeshComponent.h"
#include "Animation/AnimInstanceProxy.h"
#include "Runtime/AnimationCore/Public/TwoBoneIK.h"
#include "Utility/AnimUtil.h" 

#if WITH_EDITOR
#include "Utility/DebugDrawUtil.h"
#endif

DECLARE_CYCLE_STAT(TEXT("IK Humanoid Leg IK Trace"), STAT_IKHumanoidLegTrace_Eval, STATGROUP_Anim);

void FAnimNode_IKHumanoidLegTrace::EvaluateSkeletalControl_AnyThread(
	FComponentSpacePoseContext& Output, 
	TArray<FBoneTransform>& OutBoneTransforms)
{
	SCOPE_CYCLE_COUNTER(STAT_IKHumanoidLegTrace_Eval);

	if (Leg == nullptr || PelvisBone == nullptr)
	{
#if ENABLE_IK_DEBUG_VERBOSE
		UE_LOG(LogRTIK, Error, TEXT("An input wrapper object was null : %s"), *FString(__FUNCTION__));
#endif
		return;
	}

	USkeletalMeshComponent* SkelComp = Output.AnimInstanceProxy->GetSkelMeshComponent();
	ACharacter* Character = Cast<ACharacter>(SkelComp->GetOwner());
	const FBoneContainer& RequiredBones = Output.AnimInstanceProxy->GetRequiredBones();
	FHumanoidIK::HumanoidIKLegTrace(Character, Output.Pose, Leg->Chain, PelvisBone->Bone, MaxPelvisAdjustSize, TraceData->TraceData, false);
	TraceData->bUpdatedThisTick = true;
}
