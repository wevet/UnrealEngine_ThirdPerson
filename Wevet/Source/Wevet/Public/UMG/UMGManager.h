// Copyright 2018 wevet works All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MockCharacter.h"
#include "UMGManager.generated.h"

class UProgressBar;
class UCanvasPanel;
class UUniformGridPanel;
class UImage;

UCLASS()
class WEVET_API UUMGManager : public UUserWidget
{
	GENERATED_BODY()
	
	
public:
	UUMGManager(const FObjectInitializer& ObjectInitializer);

	virtual void NativeConstruct() override;
	void Init(ACharacterBase* NewCharacter);
	
protected:
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UMGManager|Variable")
	class ACharacterBase* CharacterOwner;

	class UImage* RadialProgressImage;
	class UCanvasPanel* BasePanel;
	class UCanvasPanel* FocusPanel;
	class UUniformGridPanel* WeaponGridPanel;
	class UImage* WeaponItemImage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UMGManager|Variable")
	FName RadialProgressImageKeyName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UMGManager|Variable")
	FName BasePanelKeyName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UMGManager|Variable")
	FName FocusPanelKeyName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UMGManager|Variable")
	FName WeaponGridPanelKeyName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UMGManager|Variable")
	FName WeaponItemImageKeyName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UMGManager|MaterialParameter")
	FName HealthScalarParameterValueName;

	virtual void SetHealth();
	virtual void SetVisibilityWeapon();
	virtual void SetVisibilityWeaponDetail();
	bool bHasWeapon;
};
