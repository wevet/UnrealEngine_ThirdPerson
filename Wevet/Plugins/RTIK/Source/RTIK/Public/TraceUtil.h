#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "CollisionQueryParams.h"
#include "Engine/World.h"
//#include "Engine.h"

#if WITH_EDITOR
#include "DebugDrawUtil.h"
#endif

#include "TraceUtil.generated.h"

UCLASS()
class RTIK_API UTraceUtil : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	
	// Do a line trace from Source to Target. Code courtesy of Rama:
	// https://wiki.unrealengine.com/Trace_Functions
	UFUNCTION(BlueprintCallable, Category = Trace)
	static bool LineTrace(UWorld* World,  AActor* ActorToIgnore, const FVector& Start, const FVector& End, FHitResult& HitOut, ECollisionChannel CollisionChannel = ECC_Pawn, bool ReturnPhysMat = false, bool bEnableDebugDraw = false)
	{
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
			FDebugDrawUtil::DrawLine(World, Start, End, FColor(0, 255, 255));
			if (bWasHitActor)
			{
				FVector HitLocation = HitOut.ImpactPoint;
				FDebugDrawUtil::DrawSphere(World, HitOut.ImpactPoint, FColor(255, 0, 0), 5.0f);
			}
		}
#endif

		return bWasHitActor;
	}
};
