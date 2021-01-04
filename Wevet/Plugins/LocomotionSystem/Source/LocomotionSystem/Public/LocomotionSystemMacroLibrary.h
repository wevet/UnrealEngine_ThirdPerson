// Copyright 2018 wevet works All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "LocomotionSystemStructs.h"
#include "Kismet/KismetMathLibrary.h"
#include "LocomotionSystemMacroLibrary.generated.h"

/**
 * 
 */
UCLASS()
class LOCOMOTIONSYSTEM_API ULocomotionSystemMacroLibrary : public UObject
{
	GENERATED_BODY()
	
public:

	UFUNCTION(BlueprintPure, Category = "LocomotionSystemMacroLibrary")
	static FLSComponentAndTransform ComponentWorldToLocal(const FLSComponentAndTransform WorldSpaceComponent)
	{
		FLSComponentAndTransform LocalSpaceComponent;
		LocalSpaceComponent.Component = WorldSpaceComponent.Component;
		FTransform Invert = UKismetMathLibrary::InvertTransform(WorldSpaceComponent.Component->K2_GetComponentToWorld());
		LocalSpaceComponent.Transform = WorldSpaceComponent.Transform * Invert;
		return LocalSpaceComponent;
	}

	UFUNCTION(BlueprintPure, Category = "LocomotionSystemMacroLibrary")
	static FLSComponentAndTransform ComponentLocalToWorld(const FLSComponentAndTransform LocalSpaceComponent)
	{
		FLSComponentAndTransform WorldSpaceComponent;
		WorldSpaceComponent.Component = LocalSpaceComponent.Component;
		WorldSpaceComponent.Transform = LocalSpaceComponent.Transform * WorldSpaceComponent.Component->K2_GetComponentToWorld();
		return WorldSpaceComponent;
	}

	UFUNCTION(BlueprintPure, Category = "LocomotionSystemMacroLibrary")
	static FTransform TransformMinus(const FTransform A, const FTransform B)
	{
		FTransform Out = FTransform::Identity;
		Out.SetLocation(A.GetLocation() - B.GetLocation());
		Out.SetScale3D(A.GetScale3D() - B.GetScale3D());

		const float Roll = (A.GetRotation().Rotator().Roll - B.GetRotation().Rotator().Roll);
		const float Pitch = (A.GetRotation().Rotator().Pitch - B.GetRotation().Rotator().Pitch);
		const float Yaw = (A.GetRotation().Rotator().Yaw - B.GetRotation().Rotator().Yaw);
		Out.SetRotation(FQuat(FRotator(Pitch, Yaw, Roll)));
		return Out;
	}

	UFUNCTION(BlueprintPure, Category = "LocomotionSystemMacroLibrary")
	static FTransform TransformPlus(const FTransform A, const FTransform B)
	{
		FTransform Out = FTransform::Identity;
		Out.SetLocation(A.GetLocation() + B.GetLocation());
		Out.SetScale3D(A.GetScale3D() + B.GetScale3D());

		const float Roll = (A.GetRotation().Rotator().Roll + B.GetRotation().Rotator().Roll);
		const float Pitch = (A.GetRotation().Rotator().Pitch + B.GetRotation().Rotator().Pitch);
		const float Yaw = (A.GetRotation().Rotator().Yaw + B.GetRotation().Rotator().Yaw);
		Out.SetRotation(FQuat(FRotator(Pitch, Yaw, Roll)));
		return Out;
	}

	UFUNCTION(BlueprintPure, Category = "LocomotionSystemMacroLibrary")
	static uint8 DoifDifferentByte(const uint8 A, const uint8 B) 
	{
		return DoifDifferent<uint8>(A, B); 
	}

	UFUNCTION(BlueprintPure, Category = "LocomotionSystemMacroLibrary")
	static bool DoifDifferentBool(const bool A, const bool B) 
	{
		return DoifDifferent<bool>(A, B); 
	}

	UFUNCTION(BlueprintPure, Category = "LocomotionSystemMacroLibrary")
	static float DoifDifferentFloat(const float A, const float B)  
	{
		return DoifDifferent<float>(A, B); 
	}

	template<typename T>
	static T DoifDifferent(const T A, const T B) 
	{
		if (A != B)
		{
			return A;
		}
		return B;
	}
};
