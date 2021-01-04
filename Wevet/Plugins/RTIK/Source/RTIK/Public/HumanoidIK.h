
#pragma once

#include "RTIK.h"
#include "CoreMinimal.h"
#include "IKTypes.h"
#include "Constraints.h"
#include "BonePose.h"
#include "Animation/AnimNodeBase.h"
#include "BoneControllers/AnimNode_SkeletalControlBase.h"
#include "Components/SkeletalMeshComponent.h"
#include "HumanoidIK.generated.h"


struct FIKUtil
{
public:
	static FVector IKBoneAxisToVector(EIKBoneAxis InBoneAxis)
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

	static FVector GetSkeletalMeshWorldAxis(const USkeletalMeshComponent& SkelComp, EIKBoneAxis InBoneAxis)
	{
		FTransform ComponentTransform = SkelComp.GetComponentToWorld();
		switch (InBoneAxis)
		{
			case EIKBoneAxis::IKBA_X: return ComponentTransform.GetUnitAxis(EAxis::X);
			case EIKBoneAxis::IKBA_Y: return ComponentTransform.GetUnitAxis(EAxis::Y);
			case EIKBoneAxis::IKBA_Z: return ComponentTransform.GetUnitAxis(EAxis::Z);
			case EIKBoneAxis::IKBA_XNeg: return -1 * ComponentTransform.GetUnitAxis(EAxis::X);
			case EIKBoneAxis::IKBA_YNeg: return -1 * ComponentTransform.GetUnitAxis(EAxis::Y);
			case EIKBoneAxis::IKBA_ZNeg: return -1 * ComponentTransform.GetUnitAxis(EAxis::Z);
		}
		return FVector(0.0f, 0.0f, 0.0f);
	}
};

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
	{ }

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
	bool InitIfInvalid(const FBoneContainer& RequiredBones)
	{
		if (IsValid(RequiredBones))
		{
			return true;
		}
		bool bIsValid = Init(RequiredBones);
		return bIsValid;
	}

	bool Init(const FBoneContainer& RequiredBones)
	{
		FIKBoneConstraint* BoneConstraint = GetConstraint();
		if (BoneConstraint != nullptr && !BoneConstraint->Initialize())
		{
#if ENABLE_IK_DEBUG
			UE_LOG(LogRTIK, Warning, TEXT("BoneConstraint failed to initialization \n bone %s funcName : %s"), *BoneRef.BoneName.ToString(), *FString(__FUNCTION__));
#endif
		}
		if (BoneRef.Initialize(RequiredBones))
		{
			BoneIndex = BoneRef.GetCompactPoseIndex(RequiredBones);
			return true;
		}
		else
		{
#if ENABLE_IK_DEBUG
			UE_LOG(LogRTIK, Warning, TEXT("BoneRef failed to initialization \n bone: %s : funName : %s"), *BoneRef.BoneName.ToString(), *FString(__FUNCTION__));
#endif
			return false;
		}
	}

	bool IsValid(const FBoneContainer& RequiredBones)
	{
		bool bValid = BoneRef.IsValidToEvaluate(RequiredBones);
#if ENABLE_IK_DEBUG_VERBOSE
		if (!bValid)
		{
			UE_LOG(LogRTIK, Warning, TEXT("BoneRef notvalid \n bone : %s : funcName : %s"), *BoneRef.BoneName.ToString(), *FString(__FUNCTION__));
		}
#endif
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
	virtual bool InitIfInvalid(const FBoneContainer& RequiredBones)
	{
		if (IsValid(RequiredBones))
		{
			return true;
		}
		InitBoneReferences(RequiredBones);
		bool bValid = IsValid(RequiredBones);
		return bValid;
	}

	virtual bool InitBoneReferences(const FBoneContainer& RequiredBones)
	{
		return false;
	}

	virtual bool IsValid(const FBoneContainer& RequiredBones)
	{
		return false;
	}
};


USTRUCT(BlueprintType)
struct RTIK_API FHumanoidLegChain : public FIKModChain
{
	GENERATED_USTRUCT_BODY()

public:
	FHumanoidLegChain()
	{
		FootRadius = 12.5f;
		ToeRadius = 5.f;
		MaxFootRotationDegrees = 40.f;
		TotalChainLength = 0.f;
		bIsInitialized = false;
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	float FootRadius;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	float ToeRadius;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	FIKBone HipBone;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	FIKBone ThighBone;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	FIKBone ShinBone;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	FIKBone FootBone;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	float MaxFootRotationDegrees;

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
		FVector ToCS = -1 * SkelComp.GetComponentLocation();
		FVector FootFloorCS = ToCS + TraceData.FootHitResult.ImpactPoint;
		FVector ToeFloorCS = ToCS + TraceData.ToeHitResult.ImpactPoint;
		FVector FloorSlopeVec = ToeFloorCS - FootFloorCS;
		FVector FloorFlatVec(FloorSlopeVec);
		FloorFlatVec.Z = 0.0f;

		if (!FloorSlopeVec.Normalize() || !FloorFlatVec.Normalize())
		{
			OutAngleRad = 0.0f;
			return false;
		}

		OutAngleRad = FMath::Acos(FVector::DotProduct(FloorFlatVec, FloorSlopeVec));
		float RequiredRotationDeg = FMath::RadiansToDegrees(OutAngleRad);
		if (RequiredRotationDeg > MaxFootRotationDegrees)
		{
			return false;
		}
		return true;
	}

	bool GetIKFloorPointCS(const USkeletalMeshComponent& SkelComp, const FHumanoidIKTraceData& TraceData, FVector& OutFloorLocationCS) const
	{
		FVector ToCS = -1 * SkelComp.GetComponentLocation();
		FVector FootFloorCS = ToCS + TraceData.FootHitResult.ImpactPoint;
		FVector ToeFloorCS = ToCS + TraceData.ToeHitResult.ImpactPoint;
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

#if ENABLE_IK_DEBUG_VERBOSE
			else
			{
				UE_LOG(LogRTIK, Warning, TEXT("Warning, GetIKFloorPointCS was called on an invalid trace result. The output floor point may be invalid."));
			}
#endif 
			return false;
		}

		float Unused;
		bool bWithinRotationLimit = FindWithinFootRotationLimit(SkelComp, TraceData, Unused);
		if (bWithinRotationLimit)
		{
			OutFloorLocationCS = FootFloorCS;
		}
		else
		{
			OutFloorLocationCS = FootFloorCS.Z > ToeFloorCS.Z ? FootFloorCS : ToeFloorCS;
		}
		return bWithinRotationLimit;
	}

	virtual bool InitBoneReferences(const FBoneContainer& RequiredBones) override
	{
		TotalChainLength = 0.0f;
		bIsInitialized = true;

		if (!HipBone.Init(RequiredBones))
		{
#if ENABLE_IK_DEBUG
			UE_LOG(LogRTIK, Warning, TEXT("Hip Bone invalid : %s"), *FString(__FUNCTION__));
#endif
			bIsInitialized = false;
		}

		if (!ThighBone.Init(RequiredBones))
		{
#if ENABLE_IK_DEBUG
			UE_LOG(LogRTIK, Warning, TEXT("Thigh Bone invalid : %s"), *FString(__FUNCTION__));
#endif
			bIsInitialized = false;
		}

		if (!ShinBone.Init(RequiredBones))
		{
#if ENABLE_IK_DEBUG
			UE_LOG(LogRTIK, Warning, TEXT("ShinBone invalid : %s"), *FString(__FUNCTION__));
#endif
			bIsInitialized = false;
		}

		if (!FootBone.Init(RequiredBones))
		{
#if ENABLE_IK_DEBUG
			UE_LOG(LogRTIK, Warning, TEXT("FootBone invalid : %s"), *FString(__FUNCTION__));
#endif
			bIsInitialized = false;
		}

		if (bIsInitialized)
		{
			const TArray<FTransform>& RefTransforms = RequiredBones.GetRefPoseArray();
			FVector HipLoc = RefTransforms[HipBone.BoneRef.BoneIndex].GetLocation();
			FVector KneeLoc = RefTransforms[ThighBone.BoneRef.BoneIndex].GetLocation();
			FVector AnkleLoc = RefTransforms[ShinBone.BoneRef.BoneIndex].GetLocation();
			FVector ToeLoc = RefTransforms[FootBone.BoneRef.BoneIndex].GetLocation();
			FVector ThighVec = KneeLoc - HipLoc;
			FVector ShinVec = AnkleLoc - KneeLoc;
			FVector FootVec = ToeLoc - AnkleLoc;
			float ThighSize = ThighVec.Size();
			float ShinSize = ShinVec.Size();
			float FootSize = FootVec.Size();
			TotalChainLength = ThighSize + ShinSize + FootSize;
		}
		return bIsInitialized;
	}

	virtual bool IsValid(const FBoneContainer& RequiredBones) override
	{
		const bool bValid = HipBone.IsValid(RequiredBones) &&
			ThighBone.IsValid(RequiredBones) &&
			ShinBone.IsValid(RequiredBones) &&
			FootBone.IsValid(RequiredBones);
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
	FRangeLimitedIKChain() : bValid(false)
	{ }

	virtual ~FRangeLimitedIKChain()
	{ }

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RangeLimitedIK")
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
	
	size_t Num()
	{
		return BonesRootToEffector.Num();
	}

	virtual bool InitBoneReferences(const FBoneContainer& RequiredBones) override
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
#if ENABLE_IK_DEBUG
					UE_LOG(LogRTIK, Warning, TEXT("Could not initialized range limited IK chain - bone named %s was not preceeded by a skeletal parent : funcName : %s"),
						*(Bone.BoneRef.BoneName.ToString()),
						*FString(__FUNCTION__));
#endif
					bValid = false;
				}
				FTransform BoneTransform = RequiredBones.GetRefPoseTransform(Bone.BoneIndex);
				FTransform ParentTransform = RequiredBones.GetRefPoseTransform(PreviousBone.BoneIndex);
				if (FVector::Dist(BoneTransform.GetLocation(), ParentTransform.GetLocation()) < KINDA_SMALL_NUMBER)
				{
#if ENABLE_IK_DEBUG
					UE_LOG(LogRTIK, Warning, TEXT("Could not initialized range limited IK chain - bone named %s has zero length : funcName : %s"),
						*(Bone.BoneRef.BoneName.ToString()),
						*FString(__FUNCTION__));
#endif
					bValid = false;
				}
			}
			if (Bone.BoneIndex.GetInt() > LargestBoneIndex)
			{
				LargestBoneIndex = Bone.BoneIndex.GetInt();
			}
		}
		return bValid;
	}
	
	virtual bool IsValid(const FBoneContainer& RequiredBones) override
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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings)
	FIKBone Bone;

public:
	UFUNCTION(BlueprintCallable, Category = IK)
	void Initialize(FIKBone InBone)
	{
		Bone = InBone;
		bInitialized = true;
	}

	bool InitIfInvalid(const FBoneContainer& RequiredBones)
	{
		if (!bInitialized)
		{
#if ENABLE_IK_DEBUG
			UE_LOG(LogRTIK, Warning, TEXT("IK Bone Wrapper was not initialized -- you must call Initialize in blueprint before use : %s"), *FString(__FUNCTION__));
#endif
			return false;
		}
		return Bone.InitIfInvalid(RequiredBones);
	}

	bool Init(const FBoneContainer& RequiredBones)
	{
		if (!bInitialized)
		{
#if ENABLE_IK_DEBUG
			UE_LOG(LogRTIK, Warning, TEXT("IK Bone Wrapper was not initialized -- you must call Initialize in blueprint before use : %s"), *FString(__FUNCTION__));
#endif
			return false;
		}
		return Bone.Init(RequiredBones);
	}

	bool IsValid(const FBoneContainer& RequiredBones)
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

	virtual bool InitIfInvalid(const FBoneContainer& RequiredBones)
	{
		return false;
	}

	virtual bool InitBoneReferences(const FBoneContainer& RequiredBones)
	{
		return false;
	}

	virtual bool IsValid(const FBoneContainer& RequiredBones)
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
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings)
	FRangeLimitedIKChain Chain;

	UFUNCTION(BlueprintCallable, Category = IK)
	void Initialize(FRangeLimitedIKChain InChain)
	{
		Chain = InChain;
		bInitialized = true;
	}

	virtual bool InitIfInvalid(const FBoneContainer& RequiredBones)
	{
		if (!bInitialized)
		{
#if ENABLE_IK_DEBUG
			UE_LOG(LogRTIK, Warning, TEXT("Range limited IK chain wrapper was not initialized -- make sure you call Initialize function in blueprint before use : %s"), *FString(__FUNCTION__));
#endif
			return false;
		}
		return Chain.InitIfInvalid(RequiredBones);
	}

	virtual bool InitBoneReferences(const FBoneContainer& RequiredBones)
	{
		if (!bInitialized)
		{
#if ENABLE_IK_DEBUG
			UE_LOG(LogRTIK, Warning, TEXT("Range limited IK chain wrapper was not initialized -- make sure you call Initialize function in blueprint before use : %s"), *FString(__FUNCTION__));
#endif
			return false;
		}
		return Chain.InitIfInvalid(RequiredBones);
	}

	virtual bool IsValid(const FBoneContainer& RequiredBones)
	{
		if (!bInitialized)
		{
			return false;
		}
		return Chain.IsValid(RequiredBones);
	}
};


UCLASS(BlueprintType, EditInlineNew)
class RTIK_API UHumanoidLegChain_Wrapper : public UIKChainWrapper
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings)
	FHumanoidLegChain Chain;

	UFUNCTION(BlueprintCallable, Category = IK)
		void Initialize(FHumanoidLegChain InChain)
	{
		Chain = InChain;
		bInitialized = true;
	}

	virtual bool InitIfInvalid(const FBoneContainer& RequiredBones)
	{
		if (!bInitialized)
		{
#if ENABLE_IK_DEBUG
			UE_LOG(LogRTIK, Warning, TEXT("Humanoid IK Leg Chain wrapper was not initialized -- make sure you call Initialize function in blueprint before use"));
#endif
			return false;
		}

		return Chain.InitIfInvalid(RequiredBones);
	}

	virtual bool InitBoneReferences(const FBoneContainer& RequiredBones)
	{
		if (!bInitialized)
		{
#if ENABLE_IK_DEBUG
			UE_LOG(LogRTIK, Warning, TEXT("Humanoid IK Leg Chain wrapper was not initialized -- make sure you call Initialize function in blueprint before use"));
#endif
			return false;
		}

		return Chain.InitIfInvalid(RequiredBones);
	}

	virtual bool IsValid(const FBoneContainer& RequiredBones)
	{
		if (!bInitialized)
		{
			return false;
		}
		return Chain.IsValid(RequiredBones);
	}
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

	UFUNCTION(BlueprintCallable, Category = IK)
	FHumanoidIKTraceData& GetTraceData()
	{
#if ENABLE_IK_DEBUG
		if (!bUpdatedThisTick)
		{
			UE_LOG(LogRTIK, Warning, TEXT("Warning -- Trace data was used before it was updated and may be stale. Use a trace node (e.g., IK Humanoid Leg Trace) to update your trace data early in the animgraph, before it is used!"));
		}
#endif
		return TraceData;
	}
	friend struct FAnimNode_IKHumanoidLegTrace;

protected:
	bool bUpdatedThisTick;
	FHumanoidIKTraceData TraceData;
};


USTRUCT()
struct RTIK_API FHumanoidIK
{
	GENERATED_USTRUCT_BODY()

public:
	static void HumanoidIKLegTrace(ACharacter* Character, FCSPose<FCompactPose>& MeshBases, FHumanoidLegChain& LegChain, FIKBone& PelvisBone, float MaxPelvisAdjustHeight, FHumanoidIKTraceData& OutTraceData, bool bEnableDebugDraw = false);
};


