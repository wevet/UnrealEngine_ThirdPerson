
#include "Constraints.h"
#include "RTIK.h"

#if WITH_EDITOR
#include "Utility/DebugDrawUtil.h"
#endif


void FPlanarRotation::EnforceConstraint(
	int32 Index, 
	const TArray<FTransform>& ReferenceCSTransforms, 
	const TArray<FIKBoneConstraint*>& Constraints, 
	TArray<FTransform>& CSTransforms, 
	ACharacter* Character)
{
	int32 NumBones = CSTransforms.Num();
	if (Index >= NumBones - 1)
	{
#if ENABLE_IK_DEBUG_VERBOSE
		UE_LOG(LogRTIK, Warning, TEXT("IK: Can't use planar joint constraint on effector bone : %s"), *FString(__FUNCTION__));
#endif
		return;
	}

	FVector UpDirection = FVector::CrossProduct(RotationAxis, ForwardDirection);

#if ENABLE_IK_DEBUG_VERBOSE
	if (!RotationAxis.IsNormalized() || !ForwardDirection.IsNormalized() || !UpDirection.IsNormalized())
	{
		UE_LOG(LogRTIK, Warning, TEXT("Planar rotation constraint contained an unnormalized direction : %s"), *FString(__FUNCTION__));
	}
#endif 

	FVector ParentLoc = CSTransforms[Index].GetLocation();
	FVector ChildLoc = CSTransforms[Index + 1].GetLocation();

	// Step 1: project onto rotation plane
	FVector BoneDirection = FVector::VectorPlaneProject((ChildLoc - ParentLoc), RotationAxis);
	float BoneLength = (ChildLoc - ParentLoc).Size();

	if (!BoneDirection.Normalize())
	{
		BoneDirection = FailsafeDirection;
	}

	// Step 2: Find the current angle
	float AngleRad = (FVector::DotProduct(BoneDirection, UpDirection) > 0.0f) ?
		FMath::Acos(FVector::DotProduct(BoneDirection, ForwardDirection)) :
		-1 * FMath::Acos(FVector::DotProduct(BoneDirection, ForwardDirection));

	float TargetDeg = FMath::Clamp(FMath::RadiansToDegrees(AngleRad), MinDegrees, MaxDegrees);
	BoneDirection = ForwardDirection.RotateAngleAxis(TargetDeg, RotationAxis);
	BoneDirection *= BoneLength;

	CSTransforms[Index + 1].SetLocation(ParentLoc + BoneDirection);

#if WITH_EDITOR
	if (bEnableDebugDraw && Character != nullptr)
	{
		UWorld* World = Character->GetWorld();
		FMatrix ToWorld = Character->GetMesh()->GetComponentToWorld().ToMatrixNoScale();

		FDebugDrawUtil::DrawVector(World, ToWorld.TransformPosition(ParentLoc), ToWorld.TransformVector(BoneDirection), FColor(255, 255, 0));
		FDebugDrawUtil::DrawVector(World, ToWorld.TransformPosition(ParentLoc), ToWorld.TransformVector(ForwardDirection), FColor(255, 0, 0));
		FDebugDrawUtil::DrawVector(World, ToWorld.TransformPosition(ParentLoc), ToWorld.TransformVector(RotationAxis), FColor(0, 255, 0));
		FDebugDrawUtil::DrawVector(World, ToWorld.TransformPosition(ParentLoc), ToWorld.TransformVector(UpDirection), FColor(0, 0, 255));

		// Draw a debug 'cone'
		FVector MaxRotation = ForwardDirection.RotateAngleAxis(MaxDegrees, RotationAxis);
		FVector MinRotation = ForwardDirection.RotateAngleAxis(MinDegrees, RotationAxis);

		FDebugDrawUtil::DrawVector(World, ToWorld.TransformPosition(ParentLoc), ToWorld.TransformVector(MaxRotation), FColor(0, 255, 255));
		FDebugDrawUtil::DrawVector(World, ToWorld.TransformPosition(ParentLoc), ToWorld.TransformVector(MinRotation), FColor(0, 255, 255));
		FString AngleStr = FString::Printf(TEXT("%f / %f"), FMath::RadiansToDegrees(AngleRad), TargetDeg);
		FDebugDrawUtil::DrawString(World, FVector(0.0f, 0.0f, 100.0f), AngleStr, Character, FColor(0, 0, 255));
	}
#endif
}

void FPlanarRotation::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	//
}