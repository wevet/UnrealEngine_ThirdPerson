// Copyright 2018 wevet works All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GrabInstigator.generated.h"

class USoundBase;

/*
* Climb System interface
*/
UINTERFACE(BlueprintType)
class WEVET_API UGrabInstigator : public UInterface
{
	GENERATED_UINTERFACE_BODY()
};

class WEVET_API IGrabInstigator
{
	GENERATED_IINTERFACE_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "GrabInstigator")
	void CanGrab(bool InCanGrab);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "GrabInstigator")
	void ClimbLedge(bool InClimbLedge);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "GrabInstigator")
	void ReportClimbEnd();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "GrabInstigator")
	void ClimbMove(float Value);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "GrabInstigator")
	void ClimbJump();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "GrabInstigator")
	void ReportClimbJumpEnd();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "GrabInstigator")
	void TurnConerLeftUpdate();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "GrabInstigator")
	void TurnConerRightUpdate();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "GrabInstigator")
	void TurnConerResult();
};
