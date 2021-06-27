
#include "AnimNode_IKHumanoidLegTrace.h"
#include "RTIK.h"
#include "Components/SkeletalMeshComponent.h"
#include "Animation/AnimInstanceProxy.h"
#include "Runtime/AnimationCore/Public/TwoBoneIK.h"
#include "IKFunctionLibrary.h" 


DECLARE_CYCLE_STAT(TEXT("IK Humanoid Leg IK Trace"), STAT_IKHumanoidLegTrace_Eval, STATGROUP_Anim);


void FAnimNode_IKHumanoidLegTrace::EvaluateSkeletalControl_AnyThread(FComponentSpacePoseContext& Output, TArray<FBoneTransform>& OutBoneTransforms)
{
	SCOPE_CYCLE_COUNTER(STAT_IKHumanoidLegTrace_Eval);

	if (Leg == nullptr || PelvisBone == nullptr || TraceData == nullptr)
	{
#if ENABLE_IK_DEBUG_VERBOSE
		UE_LOG(LogNIK, Error, TEXT("An input wrapper object was null : %s"), *FString(__FUNCTION__));
#endif
		return;
	}

	USkeletalMeshComponent* Component = Output.AnimInstanceProxy->GetSkelMeshComponent();
	ACharacter* Character = Cast<ACharacter>(Component->GetOwner());
	const FBoneContainer& RequiredBones = Output.AnimInstanceProxy->GetRequiredBones();

	FHumanoidIK::HumanoidIKLegTrace(
		Character, 
		Output.Pose, 
		TraceData->GetTraceData(),
		Leg->GetChain(), 
		PelvisBone->Bone,
		MaxPelvisAdjustSize, 
		bEnableDebugDraw);

	TraceData->SetUpdatedThisTick(true);
}
