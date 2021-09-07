#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNodeBase.h"
#include "Animation/AnimNodeSpaceConversions.h"
#include "BoneControllers/AnimNode_ModifyBone.h"
#include "BoneControllers/AnimNode_AnimDynamics.h"
#include "BoneContainer.h"
#include "BonePose.h"
#include "BoneControllers/AnimNode_SkeletalControlBase.h"
#include "AnimNode_KawaiiPhysics.generated.h"


class UKawaiiPhysicsLimitsDataAsset;


UENUM()
enum class EPlanarConstraint : uint8
{
	None,
	X,
	Y,
	Z,
};


UENUM()
enum class EBoneForwardAxis : uint8
{
	X_Positive,
	X_Negative,
	Y_Positive,
	Y_Negative,
	Z_Positive,
	Z_Negative,
};


UENUM()
enum class ECollisionLimitType : uint8
{
	None,
	Spherical,
	Capsule,
	Planar,
};


USTRUCT()
struct FCollisionLimitBase
{
	GENERATED_BODY();

	UPROPERTY(EditAnywhere, Category = CollisionLimitBase)
	FBoneReference DrivingBone;

	UPROPERTY(EditAnywhere, Category = CollisionLimitBase)
	FVector OffsetLocation = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, Category = CollisionLimitBase, meta = (ClampMin = "-360", ClampMax = "360"))
	FRotator OffsetRotation = FRotator::ZeroRotator;

	UPROPERTY()
	FVector Location = FVector::ZeroVector;

	UPROPERTY()
	FQuat Rotation = FQuat::Identity;

#if WITH_EDITORONLY_DATA

	UPROPERTY()
	bool bFromDataAsset = false;

	UPROPERTY(VisibleAnywhere, Category = Debug, meta = (IgnoreForMemberInitializationTest))
	FGuid Guid = FGuid::NewGuid();

	UPROPERTY()
	ECollisionLimitType Type = ECollisionLimitType::None;

#endif

};

USTRUCT()
struct FSphericalLimit : public FCollisionLimitBase
{
	GENERATED_BODY();

	FSphericalLimit()
	{
#if WITH_EDITORONLY_DATA
		Type = ECollisionLimitType::Spherical;
#endif
	}

	UPROPERTY(EditAnywhere, Category = SphericalLimit, meta = (ClampMin = "0"))
	float Radius = 5.0f;

	UPROPERTY(EditAnywhere, Category = SphericalLimit)
	ESphericalLimitType LimitType = ESphericalLimitType::Outer;
};


USTRUCT()
struct FCapsuleLimit : public FCollisionLimitBase
{
	GENERATED_BODY();


	FCapsuleLimit()
	{
#if WITH_EDITORONLY_DATA
		Type = ECollisionLimitType::Capsule;
#endif
	}

	UPROPERTY(EditAnywhere, Category = CapsuleLimit, meta = (ClampMin = "0"))
	float Radius = 5.0f;

	UPROPERTY(EditAnywhere, Category = CapsuleLimit, meta = (ClampMin = "0"))
	float Length = 10.0f;
};


USTRUCT()
struct FPlanarLimit : public FCollisionLimitBase
{
	GENERATED_BODY();

	FPlanarLimit()
	{
#if WITH_EDITORONLY_DATA
		Type = ECollisionLimitType::Planar;
#endif
	}

	UPROPERTY()
	FPlane Plane = FPlane(0, 0, 0, 0);
};


USTRUCT(BlueprintType)
struct KAWAIIPHYSICS_API FKawaiiPhysicsSettings
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0"), category = "KawaiiPhysics")
	float Damping = 0.1f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0"), category = "KawaiiPhysics")
	float WorldDampingLocation = 0.8f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0"), category = "KawaiiPhysics")
	float WorldDampingRotation = 0.8f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0"), category = "KawaiiPhysics")
	float Stiffness = 0.05f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0"), category = "KawaiiPhysics")
	float Radius = 3.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (PinHiddenByDefault, ClampMin = "0"), category = "KawaiiPhysics")
	float LimitAngle = 0.0f;
};


USTRUCT()
struct KAWAIIPHYSICS_API FKawaiiPhysicsModifyBone
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY()
	FBoneReference BoneRef;

	UPROPERTY()
	int ParentIndex = -1;
	
	UPROPERTY()
	TArray<int> ChildIndexs;

	UPROPERTY()
	FKawaiiPhysicsSettings PhysicsSettings;

	UPROPERTY()
	FVector Location;
	
	UPROPERTY()
	FVector PrevLocation;
	
	UPROPERTY()
	FQuat PrevRotation;
	
	UPROPERTY()
	FVector PoseLocation;
	
	UPROPERTY()
	FQuat PoseRotation;
	
	UPROPERTY()
	FVector PoseScale;
	
	UPROPERTY()
	float LengthFromRoot;
	
	UPROPERTY()
	bool bDummy = false;


public:
	void UpdatePoseTranform(const FBoneContainer& BoneContainer, FCSPose<FCompactPose>& Pose)
	{
		auto CompactPoseIndex = BoneRef.GetCompactPoseIndex(BoneContainer);
		if (CompactPoseIndex < 0)
		{
			PoseLocation = FVector::ZeroVector;
			PoseRotation = FQuat::Identity;
			PoseScale = FVector::OneVector;
			return;
		}

		auto ComponentSpaceTransform = Pose.GetComponentSpaceTransform(CompactPoseIndex);
		PoseLocation = ComponentSpaceTransform.GetLocation();
		PoseRotation = ComponentSpaceTransform.GetRotation();
		PoseScale = ComponentSpaceTransform.GetScale3D();
	}
};


USTRUCT(BlueprintType)
struct KAWAIIPHYSICS_API FAnimNode_KawaiiPhysics : public FAnimNode_SkeletalControlBase
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere, Category = ModifyTarget)
	FBoneReference RootBone;

	UPROPERTY(EditAnywhere, Category = ModifyTarget)
	TArray<FBoneReference> ExcludeBones;

	UPROPERTY(EditAnywhere, Category = TargetFramerate, meta = (EditCondition = "OverrideTargetFramerate"))
	int TargetFramerate = 60;

	UPROPERTY(EditAnywhere, Category = TargetFramerate, meta = (InlineEditConditionToggle))
	bool OverrideTargetFramerate = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Physics Settings", meta = (PinHiddenByDefault))
	FKawaiiPhysicsSettings PhysicsSettings;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Physics Settings", meta = (PinHiddenByDefault))
	UCurveFloat* DampingCurve = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Physics Settings", meta = (PinHiddenByDefault))
	UCurveFloat* WorldDampingLocationCurve = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Physics Settings", meta = (PinHiddenByDefault))
	UCurveFloat* WorldDampingRotationCurve = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Physics Settings", meta = (PinHiddenByDefault))
	UCurveFloat* StiffnessCurve = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Physics Settings", meta = (PinHiddenByDefault))
	UCurveFloat* RadiusCurve = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Physics Settings", meta = (PinHiddenByDefault))
	UCurveFloat* LimitAngleCurve = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Advanced Physics Settings", meta = (PinHiddenByDefault))
	bool bUpdatePhysicsSettingsInGame = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DummyBone", meta = (PinHiddenByDefault, ClampMin = "0"))
	float DummyBoneLength = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DummyBone", meta = (PinHiddenByDefault))
	EBoneForwardAxis BoneForwardAxis = EBoneForwardAxis::X_Positive;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Advanced Physics Settings", meta = (PinHiddenByDefault))
	EPlanarConstraint PlanarConstraint = EPlanarConstraint::None;


	UPROPERTY(EditAnywhere, Category = "Spherical Limits")
	TArray<FSphericalLimit> SphericalLimits;

	UPROPERTY(EditAnywhere, Category = "Capsule Limits")
	TArray<FCapsuleLimit> CapsuleLimits;

	UPROPERTY(EditAnywhere, Category = "Planar Limits")
	TArray<FPlanarLimit> PlanarLimits;

	UPROPERTY(EditAnywhere, Category = "Limits Data(Experimental)")
	UKawaiiPhysicsLimitsDataAsset* LimitsDataAsset = nullptr;

	UPROPERTY(VisibleAnywhere, AdvancedDisplay, Category = "Limits Data(Experimental)")
	TArray<FSphericalLimit> SphericalLimitsData;

	UPROPERTY(VisibleAnywhere, AdvancedDisplay, Category = "Limits Data(Experimental)")
	TArray<FCapsuleLimit> CapsuleLimitsData;

	UPROPERTY(VisibleAnywhere, AdvancedDisplay, Category = "Limits Data(Experimental)")
	TArray<FPlanarLimit> PlanarLimitsData;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Teleport", meta = (PinHiddenByDefault))
	float TeleportDistanceThreshold = 300.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Teleport", meta = (PinHiddenByDefault))
	float TeleportRotationThreshold = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ExternalForce", meta = (PinHiddenByDefault))
	FVector Gravity = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Wind, meta = (PinHiddenByDefault))
	bool bEnableWind = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Wind, meta = (DisplayAfter = "bEnableWind"), meta = (PinHiddenByDefault))
	float WindScale = 1.0f;

	UPROPERTY()
	TArray< FKawaiiPhysicsModifyBone > ModifyBones;

private:
	UPROPERTY()
	float TotalBoneLength = 0;
	UPROPERTY()
	FTransform PreSkelCompTransform;
	UPROPERTY()
	bool bInitPhysicsSettings = false;

#if WITH_EDITORONLY_DATA
	UPROPERTY()
	bool bEditing = false;
#endif

	FVector SkelCompMoveVector;
	FQuat SkelCompMoveRotation;
	float DeltaTime;
	float DeltaTimeOld;
	bool bResetDynamics;

public:
	FAnimNode_KawaiiPhysics();

	virtual void Initialize_AnyThread(const FAnimationInitializeContext& Context) override;
	virtual void CacheBones_AnyThread(const FAnimationCacheBonesContext& Context) override;
	virtual bool NeedsDynamicReset() const override { return true; }
	virtual void ResetDynamics(ETeleportType InTeleportType) override;

	virtual void EvaluateSkeletalControl_AnyThread(FComponentSpacePoseContext& Output, TArray<FBoneTransform>& OutBoneTransforms) override;
	virtual bool IsValidToEvaluate(const USkeleton* Skeleton, const FBoneContainer& RequiredBones) override;
	virtual void UpdateInternal(const FAnimationUpdateContext& Context) override;

	void InitModifyBones(FComponentSpacePoseContext& Output, const FBoneContainer& BoneContainer);
	float GetTotalBoneLength()
	{
		return TotalBoneLength;
	}


private:
	FVector GetBoneForwardVector(const FQuat& Rotation)
	{
		switch (BoneForwardAxis) 
		{
			default:
			case EBoneForwardAxis::X_Positive:
			return Rotation.GetAxisX();
			case EBoneForwardAxis::X_Negative:
			return -Rotation.GetAxisX();
			case EBoneForwardAxis::Y_Positive:
			return Rotation.GetAxisY();
			case EBoneForwardAxis::Y_Negative:
			return -Rotation.GetAxisY();
			case EBoneForwardAxis::Z_Positive:
			return Rotation.GetAxisZ();
			case EBoneForwardAxis::Z_Negative:
			return -Rotation.GetAxisZ();
		}
	}

	virtual void InitializeBoneReferences(const FBoneContainer& RequiredBones) override;

	void ApplyLimitsDataAsset(const FBoneContainer& RequiredBones);

	int AddModifyBone(FComponentSpacePoseContext& Output, const FBoneContainer& BoneContainer, const FReferenceSkeleton& RefSkeleton, int BoneIndex);

	int32 CollectChildBones(const FReferenceSkeleton& RefSkeleton, int32 ParentBoneIndex, TArray<int32>& Children) const;
	void CalcBoneLength(FKawaiiPhysicsModifyBone& Bone, const TArray<FTransform>& RefBonePose);

	void UpdatePhysicsSettingsOfModifyBones();
	void UpdateSphericalLimits(TArray<FSphericalLimit>& Limits, FComponentSpacePoseContext& Output, const FBoneContainer& BoneContainer, FTransform& ComponentTransform);
	void UpdateCapsuleLimits(TArray<FCapsuleLimit>& Limits, FComponentSpacePoseContext& Output, const FBoneContainer& BoneContainer, FTransform& ComponentTransform);
	void UpdatePlanerLimits(TArray<FPlanarLimit>& Limits, FComponentSpacePoseContext& Output, const FBoneContainer& BoneContainer, FTransform& ComponentTransform);

	void SimulateModifyBones(FComponentSpacePoseContext& Output, const FBoneContainer& BoneContainer, FTransform& ComponentTransform);
	void AdjustBySphereCollision(FKawaiiPhysicsModifyBone& Bone, TArray<FSphericalLimit>& Limits);
	void AdjustByCapsuleCollision(FKawaiiPhysicsModifyBone& Bone, TArray<FCapsuleLimit>& Limits);
	void AdjustByPlanerCollision(FKawaiiPhysicsModifyBone& Bone, TArray<FPlanarLimit>& Limits);
	void AdjustByAngleLimit(FComponentSpacePoseContext& Output, const FBoneContainer& BoneContainer, FTransform& ComponentTransform, FKawaiiPhysicsModifyBone& Bone, FKawaiiPhysicsModifyBone& ParentBone);
	void AdjustByPlanarConstraint(FKawaiiPhysicsModifyBone& Bone, FKawaiiPhysicsModifyBone& ParentBone);


	void ApplySimuateResult(FComponentSpacePoseContext& Output, const FBoneContainer& BoneContainer, TArray<FBoneTransform>& OutBoneTransforms);

};

