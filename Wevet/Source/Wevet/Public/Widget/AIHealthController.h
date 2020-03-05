// Copyright G2-Studios All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AIIconWidgetBase.h"
#include "AIHealthController.generated.h"

class UProgressBar;
class UCanvasPanel;
class USizeBox;
class AAICharacterBase;

UCLASS()
class WEVET_API UAIHealthController : public UAIIconWidgetBase
{
	GENERATED_BODY()
	
public:
	UAIHealthController(const FObjectInitializer& ObjectInitializer);
	virtual void NativeConstruct() override;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UAIHealthController|Variable")
	FName HealthProgressBarKeyName;

	class UProgressBar* HealthProgressBar;
	TWeakObjectPtr<class AAICharacterBase> AICharacterPtr;

public:
	virtual void UpdateRenderingViewport() override;
	virtual void TickRenderer(const float InDeltaTime) override;
	virtual void Initializer(ACharacterBase* const NewCharacterOwner) override;
	virtual void ResetCharacterOwner() override;
};
