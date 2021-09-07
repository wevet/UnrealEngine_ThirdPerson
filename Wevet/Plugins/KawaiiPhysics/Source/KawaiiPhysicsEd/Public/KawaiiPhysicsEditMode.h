#pragma once

#include "CoreMinimal.h"
#include "UnrealWidget.h"
#include "KawaiiPhysicsEditModeBase.h"
#include "AnimNodeEditMode.h"
#include "AnimGraphNode_KawaiiPhysics.h"
#include "AnimNode_KawaiiPhysics.h"

class FEditorViewportClient;
class FPrimitiveDrawInterface;
class USkeletalMeshComponent;
struct FViewportClick;



class FKawaiiPhysicsEditMode : public FKawaiiPhysicsEditModeBase
{
public:
	FKawaiiPhysicsEditMode();

	virtual void EnterMode(class UAnimGraphNode_Base* InEditorNode, struct FAnimNode_Base* InRuntimeNode) override;
	virtual void ExitMode() override;
	virtual FVector GetWidgetLocation() const override;
	virtual UE::Widget::EWidgetMode GetWidgetMode() const override;
	virtual ECoordSystem GetWidgetCoordinateSystem() const override;
	virtual void DoTranslation(FVector& InTranslation) override;
	virtual void DoRotation(FRotator& InRotation) override;
	virtual void DoScale(FVector& InScale) override;

	virtual void Render(const FSceneView* View, FViewport* Viewport, FPrimitiveDrawInterface* PDI) override;
	virtual bool HandleClick(FEditorViewportClient* InViewportClient, HHitProxy* HitProxy, const FViewportClick& Click) override;
	virtual bool GetCustomDrawingCoordinateSystem(FMatrix& InMatrix, void* InData) override;
	virtual bool InputKey(FEditorViewportClient* InViewportClient, FViewport* InViewport, FKey InKey, EInputEvent InEvent) override;
	virtual bool ShouldDrawWidget() const override;
	virtual void DrawHUD(FEditorViewportClient* ViewportClient, FViewport* Viewport, const FSceneView* View, FCanvas* Canvas) override;


protected:
	void OnExternalNodePropertyChange(FPropertyChangedEvent& InPropertyEvent);
	FDelegateHandle NodePropertyDelegateHandle;

private:
	void RenderSphericalLimits(FPrimitiveDrawInterface* PDI);
	void RenderCapsuleLimit(FPrimitiveDrawInterface* PDI);
	void RenderPlanerLimit(FPrimitiveDrawInterface* PDI);

	FVector GetWidgetLocation(ECollisionLimitType CollisionType, int Index) const;
	UE::Widget::EWidgetMode FindValidWidgetMode(UE::Widget::EWidgetMode InWidgetMode) const;
	bool IsValidSelectCollision() const;

	FCollisionLimitBase* GetSelectCollisionLimitRuntime() const;
	FCollisionLimitBase* GetSelectCollisionLimitGraph() const;

	void DrawTextItem(FText Text, FCanvas* Canvas, float X, float& Y, float FontHeight);
	void Draw3DTextItem(FText Text, FCanvas* Canvas, const FSceneView* View, FViewport* Viewport, FVector Location);

private:
	struct FAnimNode_KawaiiPhysics* RuntimeNode;
	UAnimGraphNode_KawaiiPhysics* GraphNode;

	ECollisionLimitType SelectCollisionType = ECollisionLimitType::None;
	int SelectCollisionIndex = -1;
	bool SelectCollisionIsFromDataAsset;

	mutable UE::Widget::EWidgetMode CurWidgetMode;
};