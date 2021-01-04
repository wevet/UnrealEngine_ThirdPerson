// Copyright 2018 wevet works All Rights Reserved.
#pragma once

#include "Engine/EngineTypes.h"
#include "Animation/AnimMontage.h"
#include "Components/PrimitiveComponent.h"
#include "Curves/CurveVector.h"
#include "LocomotionSystemTypes.h"
#include "LocomotionSystemStructs.generated.h"


USTRUCT(BlueprintType)
struct LOCOMOTIONSYSTEM_API FTurnMontages
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variable")
	class UAnimMontage* TurnLAnim;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variable")
	class UAnimMontage* TurnRAnim;

public:
	FTurnMontages() : 
		TurnLAnim(nullptr),
		TurnRAnim(nullptr)
	{
	}

};


USTRUCT(BlueprintType)
struct LOCOMOTIONSYSTEM_API FCombatTurnMontages
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variable")
	struct FTurnMontages PistolTurnData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variable")
	struct FTurnMontages RifleTurnData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variable")
	struct FTurnMontages SniperRifleTurnData;

public:
	FCombatTurnMontages()
	{
	}
};


USTRUCT(BlueprintType)
struct LOCOMOTIONSYSTEM_API FPivotData
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variable")
	float PivotDirection;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variable")
	ELSMovementDirection CompletedMovementDirection;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variable")
	float CompletedStartTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variable")
	ELSMovementDirection InterruptedMovementDirection;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variable")
	float InterruptedStartTime;

public:
	FPivotData() : 
		CompletedMovementDirection(ELSMovementDirection::Forwards),
		InterruptedMovementDirection(ELSMovementDirection::Forwards)
	{
		PivotDirection = 0.0f;
		CompletedStartTime = 0.0f;
		InterruptedStartTime = 0.0f;
	}

};


USTRUCT(BlueprintType)
struct LOCOMOTIONSYSTEM_API FCameraSettings
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variable")
	float TargetArmLength;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variable")
	float CameraLagSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variable")
	FVector SocketOffset;

public:
	FCameraSettings() : 
		TargetArmLength(300.0f),
		CameraLagSpeed(10.0f),
		SocketOffset(0.f, 0.f, 45.f)
	{
		
	}
};


USTRUCT(BlueprintType)
struct LOCOMOTIONSYSTEM_API FCameraSettingsGait
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variable")
	struct FCameraSettings Walk;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variable")
	struct FCameraSettings Run;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variable")
	struct FCameraSettings Sprint;

public:
	FCameraSettingsGait()
	{}
};


USTRUCT(BlueprintType)
struct LOCOMOTIONSYSTEM_API FCameraSettingsStance
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variable")
	struct FCameraSettingsGait Standing;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variable")
	struct FCameraSettings Crouching;

public:
	FCameraSettingsStance()
	{}
};


USTRUCT(BlueprintType)
struct LOCOMOTIONSYSTEM_API FCameraSettingsTarget
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variable")
	struct FCameraSettingsStance VelocityDirection;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variable")
	struct FCameraSettingsStance LookingDirection;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variable")
	struct FCameraSettings Aiming;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variable")
	struct FCameraSettings Ragdoll;

public:
	FCameraSettingsTarget()
	{}
};


USTRUCT(BlueprintType)
struct LOCOMOTIONSYSTEM_API FMantleAsset
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variable")
	UAnimMontage* AnimMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variable")
	UCurveVector* Position;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variable")
	FVector StartingOffset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variable")
	float LowHeight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variable")
	float LowPlayRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variable")
	float LowStartPosition;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variable")
	float HighHeight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variable")
	float HighPlayRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variable")
	float HighStartPosition;

public:
	FMantleAsset()
	{
		AnimMontage = nullptr;
		Position = nullptr;
		StartingOffset = FVector::ZeroVector;
		LowHeight = 0.0f;
		LowPlayRate = 0.0f;
		LowStartPosition = 0.0f;
		HighHeight = 0.0f;
		HighPlayRate = 0.0f;
		HighStartPosition = 0.0f;
	}
};


USTRUCT(BlueprintType)
struct LOCOMOTIONSYSTEM_API FMantleParams
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variable")
	UAnimMontage* AnimMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variable")
	UCurveVector* Position;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variable")
	float StartingPosition;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variable")
	float PlayRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variable")
	FVector StartingOffset;

public:
	FMantleParams()
	{
		AnimMontage = nullptr;
		Position = nullptr;
		StartingPosition = 0.0f;
		PlayRate = 1.0f;
		StartingOffset = FVector::ZeroVector;
	}
};


USTRUCT(BlueprintType)
struct LOCOMOTIONSYSTEM_API FMantleTraceSettings
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variable")
	float MaxLedgeHeight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variable")
	float MinLedgeHeight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variable")
	float ReachDistance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variable")
	float ForwardTraceRadius;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variable")
	float DownwardTraceRadius;

public:
	FMantleTraceSettings()
	{
		MaxLedgeHeight = 0.0f;
		MinLedgeHeight = 0.0f;
		ReachDistance = 0.0f;
		ForwardTraceRadius = 0.0f;
		DownwardTraceRadius = 0.0f;
	}
};


USTRUCT(BlueprintType)
struct LOCOMOTIONSYSTEM_API FLSComponentAndTransform
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variable")
	FTransform Transform;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variable")
	UPrimitiveComponent* Component;


public:
	FLSComponentAndTransform()
	{
		Component = nullptr;
		Transform = FTransform::Identity;
	}
};


USTRUCT(BlueprintType)
struct LOCOMOTIONSYSTEM_API FAnimCurveCreationData
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variable")
	int32 FrameNumber;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variable")
	float CurveValue;

public:
	FAnimCurveCreationData()
	{
		FrameNumber = 0;
		CurveValue = 0.0f;
	}
};


USTRUCT(BlueprintType)
struct LOCOMOTIONSYSTEM_API FAnimCurveCreationParams
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variable")
	FName CurveName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variable")
	bool KeyEachFrame;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variable")
	TArray<struct FAnimCurveCreationData> Keys;

public:
	FAnimCurveCreationParams()
	{
		CurveName = NAME_None;
		KeyEachFrame = false;
	}

};


USTRUCT(BlueprintType)
struct LOCOMOTIONSYSTEM_API FCameraFOVParam
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variable")
	float ThirdPersonFOV;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variable")
	float FirstPersonFOV;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variable")
	bool bRightShoulder;

public:
	FCameraFOVParam()
	{
		ThirdPersonFOV = 90.f;
		FirstPersonFOV = 90.f;
		bRightShoulder = false;
	}

};


USTRUCT(BlueprintType)
struct LOCOMOTIONSYSTEM_API FCameraTraceParam
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variable")
	FVector TraceOrigin;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variable")
	float TraceRadius;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variable")
	TEnumAsByte<ETraceTypeQuery> TraceTypeQuery;

public:
	FCameraTraceParam()
	{
		TraceRadius = 10.f;
		TraceOrigin = FVector::ZeroVector;

		//auto a = UEngineTypes::ConvertToTraceType(ECC_Visibility);
	}

};
