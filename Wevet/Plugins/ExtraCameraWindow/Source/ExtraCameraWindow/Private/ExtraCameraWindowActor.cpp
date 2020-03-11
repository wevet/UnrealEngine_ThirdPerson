#include "ExtraCameraWindowActor.h"
#include "ExtraCameraWindow.h"
#include "GameDelegates.h"


AExtraCameraWindowActor::AExtraCameraWindowActor()
{
	PrimaryActorTick.bCanEverTick = true;
	MaxGridSize = 10;
}

void AExtraCameraWindowActor::BeginPlay()
{
	if (!ExtraCameraWindowEnabled)
	{
		return;
	}

	CameraManager = nullptr;

	UWorld* const World = GetWorld();

	if (World)
	{
		for (FConstPlayerControllerIterator Iterator = World->GetPlayerControllerIterator(); Iterator; ++Iterator)
		{
			APlayerController* PlayerController = Cast<APlayerController>(*Iterator);
			if (PlayerController && PlayerController->PlayerCameraManager)
			{
				CameraManager = PlayerController->PlayerCameraManager;
			}
		}
	}

	if (CameraManager == nullptr && !LockToPlayerCamera)
	{
		UE_LOG(LogTemp, Error, TEXT("ExtraCamWindow Error: No Player Camera Manager found!"));
		ExtraCameraWindowEnabled = false;
		return;
	}

	if (!LockToPlayerCamera)
	{
		GetCameraComponent()->bLockToHmd = false;
	}

	FSlateRenderer* Renderer = FSlateApplication::Get().GetRenderer();

	if (LockResolutionToMainWindow)
	{
		GEngine->GameViewport->GetViewportSize(InitialWindowSize);
	}


	SAssignNew(ExtraWindow, SWindow)
		.ClientSize(InitialWindowSize)
		.HasCloseButton(false)
		.SizingRule(LockResolutionToMainWindow ? ESizingRule::FixedSize : ESizingRule::UserSized)
		.UseOSWindowBorder(true)
		.Title(WindowTitle)
		.FocusWhenFirstShown(LockMouseFocusToExtraWindow)
		.CreateTitleBar(true);

	FSlateApplication::Get().AddWindow(ExtraWindow.ToSharedRef(), true);

	ViewportOverlayWidget = SNew(SOverlay);

	TSharedRef<SGameLayerManager> LayerManagerRef = SNew(SGameLayerManager)
		.SceneViewport(GEngine->GameViewport->GetGameViewport())
		.Visibility(EVisibility::Visible)
		.IsEnabled(true)
		.Cursor(CursorInWindow)
		[
			ViewportOverlayWidget.ToSharedRef()
		];

	TSharedPtr<SViewport> Viewport = SNew(SViewport)
		.RenderDirectlyToWindow(false)
		.EnableGammaCorrection(false)
		.EnableStereoRendering(false)
		.Cursor(CursorInWindow)
		[
			LayerManagerRef
		];


	SceneViewport = MakeShareable(new FSceneViewport(GEngine->GameViewport, Viewport));

	Viewport->SetViewportInterface(SceneViewport.ToSharedRef());

	ExtraWindow->SetContent(Viewport.ToSharedRef());
	ExtraWindow->ShowWindow();

	ExtraWindow->SetOnWindowClosed(FOnWindowClosed::CreateLambda([this](const TSharedRef<SWindow>& WindowBeingClosed)
	{
		// self extrawindow destroy
		ExtraCameraWindowEnabled = false;
		ImmediateDestroy();
	}));

	SceneViewport->CaptureMouse(LockMouseFocusToExtraWindow);
	SceneViewport->SetUserFocus(LockMouseFocusToExtraWindow);
	SceneViewport->LockMouseToViewport(LockMouseFocusToExtraWindow);

	FSlateApplication::Get().Tick();

	SceneViewport->SetOnSceneViewportResizeDel(FOnSceneViewportResize::CreateLambda([this](FVector2D NewViewportSize)
	{
		if (LockResolutionToMainWindow == false)
		{
			return;
		}
		FVector2D MainViewportSize;
		GEngine->GameViewport->GetViewportSize(MainViewportSize);

		if (MainViewportSize.X != NewViewportSize.X || MainViewportSize.Y != NewViewportSize.Y)
		{
			SceneViewport->ResizeFrame(MainViewportSize.X, MainViewportSize.Y, EWindowMode::Windowed);
		}
	}));

	StandaloneGame = false;
	if (World)
	{
		if (World->WorldType == EWorldType::Game)
		{
			StandaloneGame = true;
		}
	}

	Super::BeginPlay();
	Super::SetActorTickInterval(TickInterval);
}

bool AExtraCameraWindowActor::AddWidgetToExtraCamera(UUserWidget* InWidget, int32 ZOrder /* = -1 */)
{
	if (!ViewportOverlayWidget.IsValid())
	{
		return false;
	}

	ViewportOverlayWidget->AddSlot(ZOrder)
		[
			InWidget->TakeWidget()
		];

	return true;
}

bool AExtraCameraWindowActor::RemoveWidgetFromExtraCamera(UUserWidget* InWidget)
{
	if (!ViewportOverlayWidget.IsValid())
	{
		return false;
	}

	return ViewportOverlayWidget->RemoveSlot(InWidget->TakeWidget());
}

bool AExtraCameraWindowActor::RemoveWidgetFromExtraCameraCurrentWidget()
{
	if (UserWidget == nullptr)
	{
		return false;
	}
	if (!ViewportOverlayWidget.IsValid())
	{
		return false;
	}

	return ViewportOverlayWidget->RemoveSlot(UserWidget->TakeWidget());
}

void AExtraCameraWindowActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!ExtraCameraWindowEnabled)
	{
		return;
	}

	if (LockToPlayerCamera)
	{
		SceneViewport->Draw();
		return;
	}

	AActor* OldTarget = nullptr;
	OldTarget = CameraManager->GetViewTarget();
	CameraManager->SetViewTarget(this);
	CameraManager->UpdateCamera(0.0f);
	SceneViewport->Draw();
	CameraManager->SetViewTarget(OldTarget);
}

void AExtraCameraWindowActor::AddChildWidget(UUserWidget * ChildWidget)
{
	if (PanelWidgets.Find(ChildWidget) >= 0) 
	{
		return;
	}
	PanelWidgets.Add(ChildWidget);
}

void AExtraCameraWindowActor::BeginDestroy()
{
	if (Destroyed)
	{
		return;
	}

	Super::BeginDestroy();

	if (!ExtraCameraWindowEnabled)
	{
		return;
	}

	if (ExtraWindow.Get() != nullptr)
	{
		if (StandaloneGame == false)
		{
			ExtraWindow->RequestDestroyWindow();
		}
		else
		{
			ExtraWindow->DestroyWindowImmediately();
		}
		Destroyed = true;
	}
}

void AExtraCameraWindowActor::ImmediateDestroy()
{
	if (Destroyed)
	{
		return;
	}

	Super::Destroy();

	if (!ExtraCameraWindowEnabled)
	{
		return;
	}

	if (ExtraWindow.Get() != nullptr)
	{
		if (StandaloneGame == false)
		{
			ExtraWindow->RequestDestroyWindow();
		}
		else
		{
			ExtraWindow->DestroyWindowImmediately();
		}
		Destroyed = true;
	}
}
