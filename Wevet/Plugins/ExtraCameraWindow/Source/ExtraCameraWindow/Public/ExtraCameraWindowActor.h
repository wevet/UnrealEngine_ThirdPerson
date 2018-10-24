
#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "Templates/SubclassOf.h"
#include "Engine/EngineTypes.h"
#include "Blueprint/UserWidget.h"
#include "ExtraCameraWindowActor.generated.h"


UCLASS()
class EXTRACAMERAWINDOW_API AExtraCameraWindowActor : public ACameraActor
{
	GENERATED_BODY()

public:

	AExtraCameraWindowActor();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool ExtraCameraWindowEnabled = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float TickInterval = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool LockToPlayerCamera = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool LockMouseFocusToExtraWindow = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TEnumAsByte<EMouseCursor::Type> CursorInWindow = EMouseCursor::None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText WindowTitle = FText::FromString(TEXT("ExtraCameraWindow"));

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector2D InitialWindowSize = FVector2D(1280, 720);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool LockResolutionToMainWindow = false;

	UFUNCTION(BlueprintCallable, Category = "ExtraCameraWindow")
	bool AddWidgetToExtraCamera(UUserWidget* InWidget, int32 ZOrder = -1);

	UFUNCTION(BlueprintCallable, Category = "ExtraCameraWindow")
	bool RemoveWidgetFromExtraCamera(UUserWidget* InWidget);

	UFUNCTION(BlueprintCallable, Category = "ExtraCameraWindow")
	bool RemoveWidgetFromExtraCameraCurrentWidget();

	UFUNCTION(BlueprintCallable, Category = "ExtraCameraWindow")
	AActor* GetTargetActor() const { return CameraManager->GetViewTarget(); }

	virtual void BeginDestroy() override;

	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Category = "ExtraCameraWindow")
	void AddChildWidget(UUserWidget* ChildWidget);

	UUserWidget* GetRootWidget() const
	{
		return this->UserWidget;
	}

	virtual void ImmediateDestroy();

protected:

	virtual void BeginPlay() override;

	TSharedPtr<FSceneViewport> SceneViewport = nullptr;
	TSharedPtr<SWindow> ExtraWindow = nullptr;
	TSharedPtr<SOverlay> ViewportOverlayWidget = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = "ExtraCameraWindow")
	TArray<UUserWidget*> PanelWidgets;

	APlayerCameraManager* CameraManager;

	bool StandaloneGame = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<class UUserWidget> PanelWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<class UUserWidget> RootWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UUserWidget* UserWidget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 StartIndex;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MaxGridSize;

	bool Destroyed;
};
