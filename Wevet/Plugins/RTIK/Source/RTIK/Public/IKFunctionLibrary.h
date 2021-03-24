#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "GameFramework/Actor.h"
#include "CollisionQueryParams.h"
#include "Math/Color.h"
#include "Math/Vector.h"
#include "BonePose.h"
#include "RTIK.h"
#include "Components/SkeletalMeshComponent.h"
#include "AnimationRuntime.h"
#include "Async/Async.h"
#include "Engine/World.h"
//#include "Engine.h"

#include "IKFunctionLibrary.generated.h"

UCLASS()
class RTIK_API UIKFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	
	// Do a line trace from Source to Target. Code courtesy of Rama:
	// ttps://wiki.unrealengine.com/Trace_Functions
	UFUNCTION(BlueprintCallable, Category = IKFunctionLibrary)
	static const bool LineTrace(
		UWorld* World, 
		AActor* ActorToIgnore, 
		const FVector& Start, 
		const FVector& End, 
		FHitResult& HitOut, 
		ECollisionChannel CollisionChannel = ECC_Pawn, 
		bool ReturnPhysMat = false, 
		bool bEnableDebugDraw = false)
	{
		if (World == nullptr)
		{
			return false;
		}

		FCollisionQueryParams TraceParams(FName(TEXT("Line Trace")), true, ActorToIgnore);
		TraceParams.bTraceComplex = true;
		//TraceParams.bTraceAsyncScene = true;
		TraceParams.bReturnPhysicalMaterial = ReturnPhysMat;
		TraceParams.AddIgnoredActor(ActorToIgnore);
		HitOut = FHitResult(ForceInit);
		World->LineTraceSingleByChannel(HitOut, Start, End, CollisionChannel, TraceParams);
		const bool bWasHitActor = (HitOut.GetActor() != nullptr);

#if WITH_EDITOR
		if (bEnableDebugDraw)
		{
			DrawLine(World, Start, End, FColor(0, 255, 255));
			if (bWasHitActor)
			{
				FVector HitLocation = HitOut.ImpactPoint;
				DrawSphere(World, HitOut.ImpactPoint, FColor(255, 0, 0), 5.0f);
			}
		}
#endif

		return bWasHitActor;
	}

	// BoneのWorld座標を返す
	static const FVector GetBoneWorldLocation(USkeletalMeshComponent& SkelComp, FCSPose<FCompactPose>& MeshBases, FCompactPoseBoneIndex BoneIndex)
	{
		FTransform BoneTransform = MeshBases.GetComponentSpaceTransform(BoneIndex);
		FAnimationRuntime::ConvertCSTransformToBoneSpace(SkelComp.GetComponentTransform(), MeshBases, BoneTransform, BoneIndex, BCS_WorldSpace);
		return BoneTransform.GetLocation();
	}

	// BoneのWorld座標、回転、Scaleを返す
	static const FTransform GetBoneWorldTransform(USkeletalMeshComponent& SkelComp, FCSPose<FCompactPose>& MeshBases, FCompactPoseBoneIndex BoneIndex)
	{
		FTransform BoneTransform = MeshBases.GetComponentSpaceTransform(BoneIndex);
		FAnimationRuntime::ConvertCSTransformToBoneSpace(SkelComp.GetComponentTransform(), MeshBases, BoneTransform, BoneIndex, BCS_WorldSpace);
		return BoneTransform;
	}

	// ComponentSpaceの座標を返す
	static const FVector GetBoneCSLocation(FCSPose<FCompactPose>& MeshBases, FCompactPoseBoneIndex BoneIndex)
	{
		return MeshBases.GetComponentSpaceTransform(BoneIndex).GetLocation();
	}

	// ComponentSpaceの座標、回転、Scaleを返す
	static const FTransform GetBoneCSTransform(FCSPose<FCompactPose>& MeshBases, FCompactPoseBoneIndex BoneIndex)
	{
		return MeshBases.GetComponentSpaceTransform(BoneIndex);
	}

	// Boneの向きに応じてVectorを返す
	static const FVector IKBoneAxisToVector(EIKBoneAxis InBoneAxis)
	{
		switch (InBoneAxis)
		{
			case EIKBoneAxis::IKBA_X: return FVector(1.0f, 0.0f, 0.0f);
			case EIKBoneAxis::IKBA_Y: return FVector(0.0f, 1.0f, 0.0f);
			case EIKBoneAxis::IKBA_Z: return FVector(0.0f, 0.0f, 1.0f);
			case EIKBoneAxis::IKBA_XNeg: return FVector(-1.0f, 0.0f, 0.0f);
			case EIKBoneAxis::IKBA_YNeg: return FVector(0.0f, -1.0f, 0.0f);
			case EIKBoneAxis::IKBA_ZNeg: return FVector(0.0f, 0.0f, -1.0f);
		}
		return FVector(0.0f, 0.0f, 0.0f);
	}

	// Meshの軸のVectorを返す
	static const FVector GetSkeletalMeshWorldAxis(const USkeletalMeshComponent& SkelComp, EIKBoneAxis InBoneAxis)
	{
		const FTransform ComponentTransform = SkelComp.GetComponentToWorld();
		switch (InBoneAxis)
		{
			case EIKBoneAxis::IKBA_X: return ComponentTransform.GetUnitAxis(EAxis::X);
			case EIKBoneAxis::IKBA_Y: return ComponentTransform.GetUnitAxis(EAxis::Y);
			case EIKBoneAxis::IKBA_Z: return ComponentTransform.GetUnitAxis(EAxis::Z);
			case EIKBoneAxis::IKBA_XNeg: return -1 * ComponentTransform.GetUnitAxis(EAxis::X);
			case EIKBoneAxis::IKBA_YNeg: return -1 * ComponentTransform.GetUnitAxis(EAxis::Y);
			case EIKBoneAxis::IKBA_ZNeg: return -1 * ComponentTransform.GetUnitAxis(EAxis::Z);
		}
		return FVector::ZeroVector;
	}

	static void DrawLine(
		UWorld* World,
		const FVector& Start,
		const FVector& Finish,
		const FLinearColor& Color = FColor(0, 255, 0),
		float Duration = -1.0f,
		float Thickness = 1.0f)
	{
		AsyncTask(ENamedThreads::GameThread, [World, Start, Finish, Color, Duration, Thickness]()
		{
			UKismetSystemLibrary::DrawDebugLine(Cast<UObject>(World), Start, Finish, Color, Duration, Thickness);
		});
	}

	static void DrawSphere(
		UWorld* World,
		const FVector& Center,
		const FLinearColor& Color = FColor(0, 255, 0),
		float Radius = 15.0f,
		int32 Segments = 12,
		float Duration = -1.0f,
		float Thickness = 0.75f)
	{
		AsyncTask(ENamedThreads::GameThread, [World, Center, Radius, Segments, Color, Duration, Thickness]()
		{
			UKismetSystemLibrary::DrawDebugSphere(Cast<UObject>(World), Center, Radius, Segments, Color, Duration, Thickness);
		});
	}

	static void DrawString(
		UWorld* World,
		const FVector& Location,
		const FString& Text,
		AActor* BaseActor,
		const FColor& Color,
		float Duration = 0.0f)
	{
		AsyncTask(ENamedThreads::GameThread, [World, Location, Text, BaseActor, Color, Duration]()
		{
			UKismetSystemLibrary::DrawDebugString(Cast<UObject>(World), Location, Text, BaseActor, Color, Duration);
		});
	}

	static void DrawVector(
		UWorld* World,
		const FVector& Base,
		FVector Direction,
		const FLinearColor& Color = FColor(255, 0, 0),
		float Length = 50.0f,
		float Duration = -1.0f,
		float Thickness = 1.0f)
	{
		if (!Direction.Normalize())
		{
			return;
		}
		Direction *= Length;
		DrawLine(World, Base, Base + Direction, Color, Duration, Thickness);
	}

	static void DrawBone(
		UWorld* World,
		USkeletalMeshComponent& SkelComp,
		FCSPose<FCompactPose>& Pose,
		const FCompactPoseBoneIndex& ChildBone,
		const FLinearColor& Color = FColor(0, 255, 255),
		float Duration = -1.0f,
		float Thickness = 1.0f)
	{
		const int32 ChildIndex = ChildBone.GetInt();
		const FName ChildName = SkelComp.GetBoneName(ChildIndex);
		const FName ParentName = SkelComp.GetParentBone(ChildName);

		if (ParentName == NAME_None)
		{
			return;
		}
		int32 ParentIndex = SkelComp.GetBoneIndex(ParentName);
		FCompactPoseBoneIndex ParentBone(ParentIndex);
		const FVector ChildLocation = GetBoneWorldLocation(SkelComp, Pose, ChildBone);
		const FVector ParentLocation = GetBoneWorldLocation(SkelComp, Pose, ParentBone);
		DrawLine(World, ParentLocation, ChildLocation, Color, Duration, Thickness);
	}

	static void DrawBoneChain(
		UWorld* World,
		USkeletalMeshComponent& SkelComp,
		FCSPose<FCompactPose>& Pose,
		const FCompactPoseBoneIndex& ChainStartChild,
		const FCompactPoseBoneIndex& ChainEndParent,
		const FLinearColor& Color = FColor(0, 255, 255),
		float Duration = -1.0f,
		float Thickness = 1.0f)
	{
		int32 ParentIndex = ChainEndParent.GetInt();
		int32 ChildIndex = ChainStartChild.GetInt();
		const FName ChildName = SkelComp.GetBoneName(ChildIndex);
		const FName ParentName = SkelComp.GetBoneName(ParentIndex);

		if (!SkelComp.BoneIsChildOf(ChildName, ParentName))
		{
			return;
		}

		FName CurrChild = ChildName;
		while (CurrChild != ParentName && CurrChild != NAME_None)
		{
			FCompactPoseBoneIndex CurrBone(SkelComp.GetBoneIndex(CurrChild));
			DrawBone(World, SkelComp, Pose, CurrBone, Color, Duration, Thickness);
			CurrChild = SkelComp.GetParentBone(CurrChild);
		}
	}

};
