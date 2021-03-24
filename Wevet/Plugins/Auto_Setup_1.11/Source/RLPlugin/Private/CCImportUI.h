// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "CCImportUI.generated.h"

/**
 * TODO:
 */
UCLASS( config = EditorPerProjectUserSettings, AutoExpandCategories = ( FTransform ), HideCategories = Object, MinimalAPI )
class UCCImportUI : public UObject
{
    GENERATED_BODY()

public:
    UCCImportUI( const FObjectInitializer& kObjectInitializer );
    void WriteConfig();

private:
    void ReadConfig();

public:
	bool isCCAutoSetup;
	bool isHQSkin;
	bool isLWSkin;
	bool isStandardSkin;
	bool isCanceled;
	bool isCanChangeAutoEnable;
	bool hasCCShader;
	bool isLiveLink;
	
};
