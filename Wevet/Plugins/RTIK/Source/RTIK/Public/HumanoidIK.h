
#pragma once

#include "RTIK.h"
#include "CoreMinimal.h"
#include "IKTypes.h"
#include "IKFunctionLibrary.h"

#include "BonePose.h"
#include "Animation/AnimNodeBase.h"
#include "BoneControllers/AnimNode_SkeletalControlBase.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/Character.h"
#include "HumanoidIK.generated.h"

using EffectorCallback = TFunction<void(
	int32 Index, 
	const TArray<FTransform>& ReferenceCSTransforms, 
	const TArray<FIKBoneConstraint*>& Constraints, 
	TArray<FTransform>& CSTransforms)>;

#pragma region IKBoneConstraint
USTRUCT(BlueprintType)
struct RTIK_API FIKBoneConstraint
{
	GENERATED_USTRUCT_BODY()

public:
	FIKBoneConstraint()
	{
		bEnabled = true;
		bEnableDebugDraw = false;
	}

	virtual ~FIKBoneConstraint()
	{
	}

	void SetEnabled(const bool InEnabled)
	{
		bEnabled = InEnabled;
	}

	virtual	bool Initialize()
	{
		return true;
	}

	FORCEINLINE bool WasEnabled() const 
	{
		return bEnabled; 
	}

	// override FABRIK
	virtual void EnforceConstraint(int32 Index, const TArray<FTransform>& ReferenceCSTransforms, const TArray<FIKBoneConstraint*>& Constraints, TArray<FTransform>& CSTransforms, ACharacter* Character = nullptr)
	{
		// 
	}

	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
	{
		//
	}

	EffectorCallback SetupFn = []
	(
		int32 Index, 
		const TArray<FTransform>& ReferenceCSTransforms, 
		const TArray<FIKBoneConstraint*>& Constraints, 
		TArray<FTransform>& CSTransforms
	)
	{
		//
	};

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	bool bEnabled;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	bool bEnableDebugDraw;

};


USTRUCT(BlueprintType)
struct RTIK_API FNoBoneConstraint : public FIKBoneConstraint
{
	GENERATED_USTRUCT_BODY()

public:
	FNoBoneConstraint() : Super()
	{
	}

	~FNoBoneConstraint()
	{
	}
};


USTRUCT(BlueprintType)
struct RTIK_API FPlanarRotation : public FIKBoneConstraint
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	FVector RotationAxis;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	FVector ForwardDirection;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	FVector FailsafeDirection;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings", meta = (UIMin = -180.0f, UIMax = 180.0f))
	float MaxDegrees;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings", meta = (UIMin = -180.0f, UIMax = 180.0f))
	float MinDegrees;

public:
	FPlanarRotation() : Super(),
		RotationAxis(0.0f, 1.0f, 10.0f),
		ForwardDirection(1.0f, 0.0f, 0.0f),
		FailsafeDirection(1.0f, 0.0f, 0.0f),
		MaxDegrees(45.0f),
		MinDegrees(-45.0f)
	{
	}

	~FPlanarRotation()
	{
	}

	virtual bool Initialize() override
	{
		if (!bEnabled)
		{
#if ENABLE_IK_DEBUG_VERBOSE
			UE_LOG(LogNIK, Warning, TEXT("Planar Rotation Constraint Disabled. Please Checked Enabled"));
#endif
			return false;
		}

		bool bAxesOK = true;
		bAxesOK &= ForwardDirection.Normalize();
		bAxesOK &= RotationAxis.Normalize();
		bAxesOK &= FailsafeDirection.Normalize();

		if (!bAxesOK)
		{
#if ENABLE_IK_DEBUG_VERBOSE
			UE_LOG(LogNIK, Warning, TEXT("Planar Rotation Constraint was set up incorrectly. Forward direction direction and rotation axis must not be colinear."));
#endif
			return false;
		}
		return true;
	}

	virtual void EnforceConstraint(int32 Index, const TArray<FTransform>& ReferenceCSTransforms, const TArray<FIKBoneConstraint*>& Constraints, TArray<FTransform>& CSTransforms, ACharacter* Character = nullptr) override
	{
		if (!bEnabled)
		{
#if ENABLE_IK_DEBUG_VERBOSE
			UE_LOG(LogNIK, Warning, TEXT("Planar Rotation Constraint Disabled. Please Checked Enabled"));
#endif
			return;
		}

		const int32 NumBones = CSTransforms.Num();
		if (Index >= NumBones - 1)
		{
#if ENABLE_IK_DEBUG_VERBOSE
			UE_LOG(LogNIK, Warning, TEXT("IK: Can't use planar joint constraint on effector bone : %s"), *FString(__FUNCTION__));
#endif
			return;
		}

		FVector UpDirection = FVector::CrossProduct(RotationAxis, ForwardDirection);
#if ENABLE_IK_DEBUG_VERBOSE
		if (!RotationAxis.IsNormalized() || !ForwardDirection.IsNormalized() || !UpDirection.IsNormalized())
		{
			UE_LOG(LogNIK, Warning, TEXT("Planar rotation constraint contained an unnormalized direction : %s"), *FString(__FUNCTION__));
		}
#endif 

		const FVector ParentLoc = CSTransforms[Index].GetLocation();
		const FVector ChildLoc = CSTransforms[Index + 1].GetLocation();
		FVector BoneDirection = FVector::VectorPlaneProject((ChildLoc - ParentLoc), RotationAxis);
		const float BoneLength = (ChildLoc - ParentLoc).Size();
		if (!BoneDirection.Normalize())
		{
			BoneDirection = FailsafeDirection;
		}

		const float CurrentAngle = FMath::Acos(FVector::DotProduct(BoneDirection, ForwardDirection));
		const float UpAngle = FVector::DotProduct(BoneDirection, UpDirection);
		float AngleRad = (UpAngle > 0.0f) ? CurrentAngle : -1 * CurrentAngle;
		float TargetDeg = FMath::Clamp(FMath::RadiansToDegrees(AngleRad), MinDegrees, MaxDegrees);
		BoneDirection = ForwardDirection.RotateAngleAxis(TargetDeg, RotationAxis);
		BoneDirection *= BoneLength;
		CSTransforms[Index + 1].SetLocation(ParentLoc + BoneDirection);

#if WITH_EDITOR
		if (bEnableDebugDraw && Character != nullptr)
		{
			UWorld* World = Character->GetWorld();
			FMatrix ToWorld = Character->GetMesh()->GetComponentToWorld().ToMatrixNoScale();

			UIKFunctionLibrary::DrawVector(World, ToWorld.TransformPosition(ParentLoc), ToWorld.TransformVector(BoneDirection), FColor(255, 255, 0));
			UIKFunctionLibrary::DrawVector(World, ToWorld.TransformPosition(ParentLoc), ToWorld.TransformVector(ForwardDirection), FColor(255, 0, 0));
			UIKFunctionLibrary::DrawVector(World, ToWorld.TransformPosition(ParentLoc), ToWorld.TransformVector(RotationAxis), FColor(0, 255, 0));
			UIKFunctionLibrary::DrawVector(World, ToWorld.TransformPosition(ParentLoc), ToWorld.TransformVector(UpDirection), FColor(0, 0, 255));

			// Draw a debug 'cone'
			FVector MaxRotation = ForwardDirection.RotateAngleAxis(MaxDegrees, RotationAxis);
			FVector MinRotation = ForwardDirection.RotateAngleAxis(MinDegrees, RotationAxis);

			UIKFunctionLibrary::DrawVector(World, ToWorld.TransformPosition(ParentLoc), ToWorld.TransformVector(MaxRotation), FColor(0, 255, 255));
			UIKFunctionLibrary::DrawVector(World, ToWorld.TransformPosition(ParentLoc), ToWorld.TransformVector(MinRotation), FColor(0, 255, 255));
			FString AngleStr = FString::Printf(TEXT("%f / %f"), FMath::RadiansToDegrees(AngleRad), TargetDeg);
			UIKFunctionLibrary::DrawString(World, FVector(0.0f, 0.0f, 100.0f), AngleStr, Character, FColor(0, 0, 255));
		}
#endif
	}

};
#pragma endregion


#pragma region IKBoneConstraintWrapper
UCLASS(BlueprintType, EditInlineNew, DefaultToInstanced, abstract)
class RTIK_API UIKBoneConstraintWrapper : public UObject
{
	GENERATED_BODY()

public:
	virtual FIKBoneConstraint* GetConstraint()
	{
		return nullptr;
	}
};


UCLASS(BlueprintType, EditInlineNew, DefaultToInstanced)
class RTIK_API UNoBoneConstraintWrapper : public UIKBoneConstraintWrapper
{
	GENERATED_BODY()

public:
	virtual FIKBoneConstraint* GetConstraint() override
	{
		return &Constraint;
	}

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings)
	FNoBoneConstraint Constraint;

};


UCLASS(BlueprintType, EditInlineNew, DefaultToInstanced)
class RTIK_API UPlanarConstraintWrapper : public UIKBoneConstraintWrapper
{
	GENERATED_BODY()

public:
	virtual FIKBoneConstraint* GetConstraint() override
	{
		return &Constraint;
	}

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings)
	FPlanarRotation Constraint;

};
#pragma endregion


USTRUCT(BlueprintType)
struct RTIK_API FHumanoidIKTraceData
{
	GENERATED_USTRUCT_BODY()

public:
	FHitResult FootHitResult;
	FHitResult ToeHitResult;
};


USTRUCT(BlueprintType)
struct RTIK_API FIKBone
{
	GENERATED_USTRUCT_BODY()

public:
	FIKBone() : BoneIndex(INDEX_NONE)
	{
	}

	UPROPERTY(EditAnywhere, Category = "Settings")
	FBoneReference BoneRef;

	FIKBoneConstraint* GetConstraint()
	{
		if (Constraint == nullptr)
		{
			return nullptr;
		}
		return Constraint->GetConstraint();
	}

	FCompactPoseBoneIndex BoneIndex;

public:
	const bool InitIfInvalid(const FBoneContainer& RequiredBones)
	{
		if (IsValid(RequiredBones))
		{
			return true;
		}
		const bool bIsValid = Init(RequiredBones);
		return bIsValid;
	}

	const bool Init(const FBoneContainer& RequiredBones)
	{
		FIKBoneConstraint* BoneConstraint = GetConstraint();
		if (BoneConstraint != nullptr && !BoneConstraint->Initialize())
		{
#if ENABLE_IK_DEBUG_VERBOSE
			UE_LOG(LogNIK, Warning, TEXT("BoneConstraint failed to initialization \n bone %s funcName : %s"), *BoneRef.BoneName.ToString(), *FString(__FUNCTION__));
#endif
		}
		if (BoneRef.Initialize(RequiredBones))
		{
			BoneIndex = BoneRef.GetCompactPoseIndex(RequiredBones);
			return true;
		}
		else
		{
#if ENABLE_IK_DEBUG_VERBOSE
			UE_LOG(LogNIK, Warning, TEXT("BoneRef failed to initialization \n bone: %s : funName : %s"), *BoneRef.BoneName.ToString(), *FString(__FUNCTION__));
#endif
			return false;
		}
	}

	bool IsValid(const FBoneContainer& RequiredBones) const 
	{
		const bool bValid = BoneRef.IsValidToEvaluate(RequiredBones);
		if (!bValid)
		{
#if ENABLE_IK_DEBUG_VERBOSE
			UE_LOG(LogNIK, Warning, TEXT("BoneRef notvalid \n bone : %s : funcName : %s"), *BoneRef.BoneName.ToString(), *FString(__FUNCTION__));
#endif
		}
		return bValid;
	}

protected:
	UPROPERTY(EditAnywhere, Instanced, NoClear, Export, Category = "Settings")
	UIKBoneConstraintWrapper* Constraint;

};


#pragma region ModChain
USTRUCT(BlueprintType)
struct RTIK_API FIKModChain
{
	GENERATED_USTRUCT_BODY()

public:
	FIKModChain()
	{
	}

	virtual ~FIKModChain()
	{
	}

	virtual const bool InitIfInvalid(const FBoneContainer& RequiredBones)
	{
		if (IsValid(RequiredBones))
		{
			return true;
		}
		InitBoneReferences(RequiredBones);
		const bool bValid = IsValid(RequiredBones);
		return bValid;
	}

	virtual const bool InitBoneReferences(const FBoneContainer& RequiredBones)
	{
		return false;
	}

	virtual const bool IsValid(const FBoneContainer& RequiredBones)
	{
		return false;
	}
};


USTRUCT(BlueprintType)
struct RTIK_API FHumanoidLegChain : public FIKModChain
{
	GENERATED_USTRUCT_BODY()

public:
	FHumanoidLegChain() : Super()
	{
		FootRadius = 12.5f;
		ToeRadius = 5.f;
		MaxFootRotationDegrees = 40.f;
		TotalChainLength = 0.f;
		bIsInitialized = false;
	}

	~FHumanoidLegChain()
	{
	}


#pragma region UProperties
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	float FootRadius;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	float ToeRadius;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	float MaxFootRotationDegrees;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bones")
	FIKBone HipBone;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bones")
	FIKBone ThighBone;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bones")
	FIKBone ShinBone;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bones")
	FIKBone FootBone;
#pragma endregion


public:
	float GetTotalChainLength() const
	{
		return TotalChainLength;
	}

	bool FindWithinFootRotationLimit(const USkeletalMeshComponent& SkelComp, const FHumanoidIKTraceData& TraceData, float& OutAngleRad) const
	{
		if (TraceData.FootHitResult.GetActor() == nullptr || TraceData.ToeHitResult.GetActor() == nullptr)
		{
			return false;
		}
		const FVector ToCS = -1 * SkelComp.GetComponentLocation();
		const FVector FootFloorCS = ToCS + TraceData.FootHitResult.ImpactPoint;
		const FVector ToeFloorCS = ToCS + TraceData.ToeHitResult.ImpactPoint;
		FVector FloorSlopeVec = ToeFloorCS - FootFloorCS;
		FVector FloorFlatVec = FloorSlopeVec;
		FloorFlatVec.Z = 0.0f;

		if (!FloorSlopeVec.Normalize() || !FloorFlatVec.Normalize())
		{
			OutAngleRad = 0.0f;
			return false;
		}

		OutAngleRad = FMath::Acos(FVector::DotProduct(FloorFlatVec, FloorSlopeVec));
		const float RequiredRotationDeg = FMath::RadiansToDegrees(OutAngleRad);
		if (RequiredRotationDeg > MaxFootRotationDegrees)
		{
			return false;
		}
		return true;
	}


	bool GetIKFloorPointCS(const USkeletalMeshComponent& SkelComp, const FHumanoidIKTraceData& TraceData, FVector& OutFloorLocationCS) const
	{
		const FVector ToCS = -1 * SkelComp.GetComponentLocation();
		const FVector FootFloorCS = ToCS + TraceData.FootHitResult.ImpactPoint;
		const FVector ToeFloorCS = ToCS + TraceData.ToeHitResult.ImpactPoint;

		if (TraceData.FootHitResult.GetActor() == nullptr || TraceData.ToeHitResult.GetActor() == nullptr)
		{
			if (TraceData.FootHitResult.GetActor() != nullptr)
			{
				OutFloorLocationCS = FootFloorCS;
			}
			else if (TraceData.ToeHitResult.GetActor() != nullptr)
			{
				OutFloorLocationCS = ToeFloorCS;
			}
			else
			{
#if ENABLE_IK_DEBUG_VERBOSE
				UE_LOG(LogNIK, Warning, TEXT("Warning, GetIKFloorPointCS was called on an invalid trace result. The output floor point may be invalid."));
#endif 
			}
			return false;
		}


		float Unused;
		const bool bWithinRotationLimit = FindWithinFootRotationLimit(SkelComp, TraceData, Unused);
		if (bWithinRotationLimit)
		{
			OutFloorLocationCS = FootFloorCS;
		}
		else
		{
			OutFloorLocationCS = (FootFloorCS.Z > ToeFloorCS.Z) ? FootFloorCS : ToeFloorCS;
		}
		return bWithinRotationLimit;
	}


	virtual const bool InitBoneReferences(const FBoneContainer& RequiredBones) override
	{
		TotalChainLength = 0.0f;
		bIsInitialized = true;

		if (!HipBone.Init(RequiredBones))
		{
#if ENABLE_IK_DEBUG_VERBOSE
			UE_LOG(LogNIK, Warning, TEXT("Hip Bone invalid : %s"), *FString(__FUNCTION__));
#endif
			bIsInitialized = false;
		}

		if (!ThighBone.Init(RequiredBones))
		{
#if ENABLE_IK_DEBUG_VERBOSE
			UE_LOG(LogNIK, Warning, TEXT("Thigh Bone invalid : %s"), *FString(__FUNCTION__));
#endif
			bIsInitialized = false;
		}

		if (!ShinBone.Init(RequiredBones))
		{
#if ENABLE_IK_DEBUG_VERBOSE
			UE_LOG(LogNIK, Warning, TEXT("ShinBone invalid : %s"), *FString(__FUNCTION__));
#endif
			bIsInitialized = false;
		}

		if (!FootBone.Init(RequiredBones))
		{
#if ENABLE_IK_DEBUG_VERBOSE
			UE_LOG(LogNIK, Warning, TEXT("FootBone invalid : %s"), *FString(__FUNCTION__));
#endif
			bIsInitialized = false;
		}

		if (bIsInitialized)
		{
			const TArray<FTransform>& RefTransforms = RequiredBones.GetRefPoseArray();
			const FVector HipLoc = RefTransforms[HipBone.BoneRef.BoneIndex].GetLocation();
			const FVector KneeLoc = RefTransforms[ThighBone.BoneRef.BoneIndex].GetLocation();
			const FVector AnkleLoc = RefTransforms[ShinBone.BoneRef.BoneIndex].GetLocation();
			const FVector ToeLoc = RefTransforms[FootBone.BoneRef.BoneIndex].GetLocation();
			const FVector ThighVec = KneeLoc - HipLoc;
			const FVector ShinVec = AnkleLoc - KneeLoc;
			const FVector FootVec = ToeLoc - AnkleLoc;
			const float ThighSize = ThighVec.Size();
			const float ShinSize = ShinVec.Size();
			const float FootSize = FootVec.Size();
			TotalChainLength = ThighSize + ShinSize + FootSize;
		}
		return bIsInitialized;
	}


	virtual const bool IsValid(const FBoneContainer& RequiredBones) override
	{
		const bool bValid = HipBone.IsValid(RequiredBones) && ThighBone.IsValid(RequiredBones) && ShinBone.IsValid(RequiredBones) && FootBone.IsValid(RequiredBones);
		return bValid;
	}

protected:
	bool bIsInitialized;
	float TotalChainLength;
};


USTRUCT(BlueprintType)
struct RTIK_API FRangeLimitedIKChain : public FIKModChain
{
	GENERATED_USTRUCT_BODY()

public:
	FRangeLimitedIKChain() : Super()
	{
		bValid = false;
	}

	~FRangeLimitedIKChain()
	{

	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	TArray<FIKBone> BonesRootToEffector;
	
	FIKBone& operator[](size_t i)
	{
		return BonesRootToEffector[i];
	}
	
	FIKBone& AccessFromEffector(size_t i)
	{
		size_t NumBones = BonesRootToEffector.Num();
		return BonesRootToEffector[NumBones - 1 - i];
	}
	
	FIKBone& AccessFromRoot(size_t i)
	{
		return BonesRootToEffector[i];
	}
	
	size_t Num() const
	{
		return BonesRootToEffector.Num();
	}

	virtual const bool InitBoneReferences(const FBoneContainer& RequiredBones) override
	{
		bValid = true;

		size_t LargestBoneIndex = 0;
		for (size_t i = 0; i < BonesRootToEffector.Num(); ++i)
		{
			FIKBone& Bone = BonesRootToEffector[i];
			if (!Bone.Init(RequiredBones))
			{
				bValid = false;
			}

			if (i > 0)
			{
				FIKBone& PreviousBone = BonesRootToEffector[i - 1];
				if (PreviousBone.BoneIndex.GetInt() >= Bone.BoneIndex.GetInt())
				{
					bValid = false;
#if ENABLE_IK_DEBUG_VERBOSE
					UE_LOG(LogNIK, Warning, TEXT("Could not initialized range limited IK chain - bone named %s was not preceeded by a skeletal parent : funcName : %s"), *(Bone.BoneRef.BoneName.ToString()), *FString(__FUNCTION__));
#endif
				}
				FTransform BoneTransform = RequiredBones.GetRefPoseTransform(Bone.BoneIndex);
				FTransform ParentTransform = RequiredBones.GetRefPoseTransform(PreviousBone.BoneIndex);
				const float Distance = (BoneTransform.GetLocation() - ParentTransform.GetLocation()).Size();
				if (Distance < KINDA_SMALL_NUMBER)
				{
					bValid = false;
#if ENABLE_IK_DEBUG_VERBOSE
					UE_LOG(LogNIK, Warning, TEXT("Could not initialized range limited IK chain - bone named %s has zero length : funcName : %s"), *(Bone.BoneRef.BoneName.ToString()), *FString(__FUNCTION__));
#endif
				}
			}

			if (Bone.BoneIndex.GetInt() > LargestBoneIndex)
			{
				LargestBoneIndex = Bone.BoneIndex.GetInt();
			}
		}
		return bValid;
	}
	
	virtual const bool IsValid(const FBoneContainer& RequiredBones) override
	{
		for (FIKBone& Bone : BonesRootToEffector)
		{
			bValid &= Bone.IsValid(RequiredBones);
		}
		return bValid;
	}

protected:
	bool bValid;

};
#pragma endregion


UCLASS(BlueprintType)
class RTIK_API UIKBoneWrapper : public UObject
{
	GENERATED_BODY()

public:
	UIKBoneWrapper(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
	{
		bInitialized = false;
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	FIKBone Bone;

public:
	void Initialize(FIKBone InBone)
	{
		Bone = InBone;
		bInitialized = true;
	}

	const bool InitIfInvalid(const FBoneContainer& RequiredBones)
	{
		if (!bInitialized)
		{
#if ENABLE_IK_DEBUG_VERBOSE
			UE_LOG(LogNIK, Warning, TEXT("IK Bone Wrapper was not initialized -- you must call Initialize in blueprint before use : %s"), *FString(__FUNCTION__));
#endif
			return false;
		}
		return Bone.InitIfInvalid(RequiredBones);
	}

	const bool Init(const FBoneContainer& RequiredBones) 
	{
		if (!bInitialized)
		{
#if ENABLE_IK_DEBUG_VERBOSE
			UE_LOG(LogNIK, Warning, TEXT("IK Bone Wrapper was not initialized -- you must call Initialize in blueprint before use : %s"), *FString(__FUNCTION__));
#endif
			return false;
		}
		return Bone.Init(RequiredBones);
	}

	bool IsValid(const FBoneContainer& RequiredBones) const
	{
		if (!bInitialized)
		{
			return false;
		}
		return Bone.IsValid(RequiredBones);
	}

protected:
	bool bInitialized;
};


#pragma region IKChainWrapper
UCLASS(BlueprintType)
class RTIK_API UIKChainWrapper : public UObject
{
	GENERATED_BODY()

public:
	UIKChainWrapper(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
	{
		bInitialized = false;
	}

	virtual const bool InitIfInvalid(const FBoneContainer& RequiredBones)
	{
		return false;
	}

	virtual const bool InitBoneReferences(const FBoneContainer& RequiredBones)
	{
		return false;
	}

	virtual const bool IsValid(const FBoneContainer& RequiredBones)
	{
		return false;
	}

protected:
	bool bInitialized;
};


UCLASS(BlueprintType)
class RTIK_API URangeLimitedIKChainWrapper : public UIKChainWrapper
{
	GENERATED_BODY()

public:
	URangeLimitedIKChainWrapper(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
	{
	}

	void Initialize(FRangeLimitedIKChain InChain)
	{
		Chain = InChain;
		bInitialized = true;
	}

	FRangeLimitedIKChain GetChain() const { return Chain; }

	virtual const bool InitIfInvalid(const FBoneContainer& RequiredBones) override
	{
		if (!bInitialized)
		{
#if ENABLE_IK_DEBUG_VERBOSE
			UE_LOG(LogNIK, Warning, TEXT("Range limited IK chain wrapper was not initialized -- make sure you call Initialize function in blueprint before use : %s"), *FString(__FUNCTION__));
#endif
			return false;
		}
		return Chain.InitIfInvalid(RequiredBones);
	}

	virtual const bool InitBoneReferences(const FBoneContainer& RequiredBones) override
	{
		if (!bInitialized)
		{
#if ENABLE_IK_DEBUG_VERBOSE
			UE_LOG(LogNIK, Warning, TEXT("Range limited IK chain wrapper was not initialized -- make sure you call Initialize function in blueprint before use : %s"), *FString(__FUNCTION__));
#endif
			return false;
		}
		return Chain.InitIfInvalid(RequiredBones);
	}

	virtual const bool IsValid(const FBoneContainer& RequiredBones) override
	{
		if (!bInitialized)
		{
			return false;
		}
		return Chain.IsValid(RequiredBones);
	}

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	FRangeLimitedIKChain Chain;

};


UCLASS(BlueprintType, EditInlineNew)
class RTIK_API UHumanoidLegChain_Wrapper : public UIKChainWrapper
{
	GENERATED_BODY()

public:
	UHumanoidLegChain_Wrapper(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
	{
	}

	void Initialize(FHumanoidLegChain InChain)
	{
		Chain = InChain;
		bInitialized = true;
	}

	FHumanoidLegChain GetChain() const { return Chain; }

	virtual const bool InitIfInvalid(const FBoneContainer& RequiredBones) override
	{
		if (!bInitialized)
		{
#if ENABLE_IK_DEBUG_VERBOSE
			UE_LOG(LogNIK, Warning, TEXT("Humanoid IK Leg Chain wrapper was not initialized -- make sure you call Initialize function in blueprint before use"));
#endif
			return false;
		}
		return Chain.InitIfInvalid(RequiredBones);
	}

	virtual const bool InitBoneReferences(const FBoneContainer& RequiredBones) override
	{
		if (!bInitialized)
		{
#if ENABLE_IK_DEBUG_VERBOSE
			UE_LOG(LogNIK, Warning, TEXT("Humanoid IK Leg Chain wrapper was not initialized -- make sure you call Initialize function in blueprint before use"));
#endif
			return false;
		}
		return Chain.InitIfInvalid(RequiredBones);
	}

	virtual const bool IsValid(const FBoneContainer& RequiredBones) override
	{
		if (!bInitialized)
		{
			return false;
		}
		return Chain.IsValid(RequiredBones);
	}

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	FHumanoidLegChain Chain;

};
#pragma endregion


UCLASS(BlueprintType, EditInlineNew)
class RTIK_API UHumanoidIKTraceData_Wrapper : public UObject
{
	GENERATED_BODY()

public:
	UHumanoidIKTraceData_Wrapper(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
	{
		bUpdatedThisTick = false;
	}

	FHumanoidIKTraceData& GetTraceData()
	{
		return TraceData;
	}

	friend struct FAnimNode_IKHumanoidLegTrace;

	void SetUpdatedThisTick(const bool InUpdatedThisTick)
	{
		bUpdatedThisTick = InUpdatedThisTick;
	}

protected:
	bool bUpdatedThisTick;
	FHumanoidIKTraceData TraceData;
};


USTRUCT()
struct RTIK_API FHumanoidIK
{
	GENERATED_USTRUCT_BODY()

public:
	static void HumanoidIKLegTrace(
		ACharacter* Character, 
		FCSPose<FCompactPose>& MeshBases, 
		FHumanoidIKTraceData& OutTraceData,
		const FHumanoidLegChain LegChain, 
		const FIKBone PelvisBone,  
		const float MaxPelvisAdjustHeight, 
		const bool bEnableDebugDraw)
	{
		if (Character == nullptr)
		{
			return;
		}

		USkeletalMeshComponent* SkelComp = Character->GetMesh();
		UWorld* World = Character->GetWorld();
		const FBoneContainer& RequiredBones = MeshBases.GetPose().GetBoneContainer();
		const FVector TraceDirection = -1 * Character->GetActorUpVector();

		const FVector PelvisLocation = UIKFunctionLibrary::GetBoneCSLocation(MeshBases, PelvisBone.BoneIndex);
		const FVector FootLocation = UIKFunctionLibrary::GetBoneCSLocation(MeshBases, LegChain.ShinBone.BoneIndex);
		const FVector ToeLocation = UIKFunctionLibrary::GetBoneCSLocation(MeshBases, LegChain.FootBone.BoneIndex);
		const float TraceStartHeight = FMath::Max3(FootLocation.Z + LegChain.FootRadius, ToeLocation.Z + LegChain.ToeRadius, PelvisLocation.Z);
		const float TraceEndHeight = PelvisLocation.Z - (LegChain.GetTotalChainLength() + LegChain.FootRadius + LegChain.ToeRadius + MaxPelvisAdjustHeight);
		FVector FootTraceStart(FootLocation.X, FootLocation.Y, TraceStartHeight);
		FVector FootTraceEnd(FootLocation.X, FootLocation.Y, TraceEndHeight);
		FVector ToeTraceStart(ToeLocation.X, ToeLocation.Y, TraceStartHeight);
		FVector ToeTraceEnd(ToeLocation.X, ToeLocation.Y, TraceEndHeight);

		const FTransform ComponentToWorld = SkelComp->GetComponentToWorld();
		FootTraceStart = ComponentToWorld.TransformPosition(FootTraceStart);
		FootTraceEnd = ComponentToWorld.TransformPosition(FootTraceEnd);
		ToeTraceStart = ComponentToWorld.TransformPosition(ToeTraceStart);
		ToeTraceEnd = ComponentToWorld.TransformPosition(ToeTraceEnd);
		UIKFunctionLibrary::LineTrace(World, Character, FootTraceStart, FootTraceEnd, OutTraceData.FootHitResult, ECC_Pawn, false, bEnableDebugDraw);
		UIKFunctionLibrary::LineTrace(World, Character, ToeTraceStart, ToeTraceEnd, OutTraceData.ToeHitResult, ECC_Pawn, false, bEnableDebugDraw);
	}
};

