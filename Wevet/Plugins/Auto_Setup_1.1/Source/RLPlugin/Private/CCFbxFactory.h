// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Factories/FbxFactory.h"
#include "Runtime/SlateCore/Public/Styling/SlateTypes.h"
//#include "SlateBasics.h"
#include "Factories/Factory.h"
#include "UObject/ObjectMacros.h"
#include "CCImportUI.h"
#include "CCFbxFactory.generated.h"

class UTextureFactory;
UCLASS(hidecategories = Object)
class UCCFbxFactory : public UFbxFactory
{
    GENERATED_UCLASS_BODY()
public:

    //~ UFactory Interface
    virtual UObject* FactoryCreateFile( UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, const FString& InFilename, const TCHAR* Parms, FFeedbackContext* Warn, bool& bOutOperationCanceled );
    virtual void PostInitProperties() override;
    void ImportTextureFolder( FString& fbxRootPath, FString& rootGamePath, TArray< TCHAR* > kFileExtension, FString &fbxName, TArray<FString> &kTextureList );
    TArray<FString> GetLODPaths( FString &targetFolderPath, FString &FbxName );
    void SetShaderType( FString shaderType );
private:
    FString m_ShaderType = "Standard";
};
