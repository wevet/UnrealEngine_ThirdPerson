#pragma once

#include "RTIK.h"
#include "IKTypes.h"
#include "CoreMinimal.h"
#include "Animation/AnimNodeBase.h"
#include "AnimNode_SkeletalControlBase.h"
#include "GameFramework/Character.h"
#include "Components/SkeletalMeshComponent.h"
#include "Constraints.generated.h"


#pragma region IKBoneConstraint
USTRUCT(BlueprintType)
struct RTIK_API FIKBoneConstraint
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	bool bEnabled;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	bool bEnableDebugDraw;

public:
	FIKBoneConstraint()
	{
		bEnabled = true;
		bEnableDebugDraw = false;
	}

	virtual ~FIKBoneConstraint()
	{
	}

	virtual	bool Initialize()
	{
		return true;
	}

	virtual void EnforceConstraint(int32 Index, const TArray<FTransform>& ReferenceCSTransforms, const TArray<FIKBoneConstraint*>& Constraints, TArray<FTransform>& CSTransforms, ACharacter* Character = nullptr) 
	{ 
	}

	TFunction<void(int32 Index, const TArray<FTransform>& ReferenceCSTransforms, const TArray<FIKBoneConstraint*>& Constraints, 
		TArray<FTransform>& CSTransforms)> SetupFn = [](
			int32 Index,
			const TArray<FTransform>& ReferenceCSTransforms,
			const TArray<FIKBoneConstraint*>& Constraints,
			TArray<FTransform>& CSTransforms) 
	{
	};
};


USTRUCT(BlueprintType)
struct RTIK_API FNoBoneConstraint : public FIKBoneConstraint
{

	GENERATED_USTRUCT_BODY()

public:

	FNoBoneConstraint()
	{
	}

	bool Initialize() override
	{
		return true;
	}

	virtual void EnforceConstraint(
		int32 Index, 
		const TArray<FTransform>& ReferenceCSTransforms, 
		const TArray<FIKBoneConstraint*>& Constraints, 
		TArray<FTransform>& CSTransforms, 
		ACharacter* Character = nullptr) override
	{
	}
};


USTRUCT(BlueprintType)
struct RTIK_API FPlanarRotation : public FIKBoneConstraint
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings)
	FVector RotationAxis;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings)
	FVector ForwardDirection;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings)
	FVector FailsafeDirection;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings, meta = (UIMin = -180.0f, UIMax = 180.0f))
	float MaxDegrees;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings, meta = (UIMin = -180.0f, UIMax = 180.0f))
	float MinDegrees;

public:
	FPlanarRotation()
		: RotationAxis(0.0f, 1.0f, 10.0f),
		ForwardDirection(1.0f, 0.0f, 0.0f),
		FailsafeDirection(1.0f, 0.0f, 0.0f),
		MaxDegrees(45.0f),
		MinDegrees(-45.0f)
	{
	}

	bool Initialize() override
	{
		bool bAxesOK = true;
		bAxesOK &= ForwardDirection.Normalize();
		bAxesOK &= RotationAxis.Normalize();
		bAxesOK &= FailsafeDirection.Normalize();

		if (!bAxesOK)
		{
#if ENABLE_IK_DEBUG
			UE_LOG(LogRTIK, Warning, TEXT("Planar Rotation Constraint was set up incorrectly. Forward direction direction and rotation axis must not be colinear."));
#endif
			return false;
		}
		return true;
	}

	virtual void EnforceConstraint(int32 Index, const TArray<FTransform>& ReferenceCSTransforms, const TArray<FIKBoneConstraint*>& Constraints, TArray<FTransform>& CSTransforms, ACharacter* Character = nullptr) override;
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent);
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
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings)
	FNoBoneConstraint Constraint;

	virtual FIKBoneConstraint* GetConstraint() override
	{
		return &Constraint;
	}
};


UCLASS(BlueprintType, EditInlineNew, DefaultToInstanced)
class RTIK_API UPlanarConstraintWrapper : public UIKBoneConstraintWrapper
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings)
	FPlanarRotation Constraint;

	virtual FIKBoneConstraint* GetConstraint() override
	{
		return &Constraint;
	}
};
#pragma endregion

