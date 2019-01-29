// Copyright 2018 wevet works All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UMGManager.generated.h"

class ACharacterBase;
class UMainUIController;

UCLASS()
class WEVET_API UUMGManager : public UUserWidget
{
	GENERATED_BODY()
	
	
public:
	UUMGManager(const FObjectInitializer& ObjectInitializer);
	virtual void NativeConstruct() override;
	virtual void BeginDestroy() override;
	
protected:
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

public:
	void Initializer(ACharacterBase* const NewCharacter);

private:
	class UMainUIController* MainUIController;

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Controller)
	TSubclassOf<class UMainUIController> MainUIControllerTemp;
};

