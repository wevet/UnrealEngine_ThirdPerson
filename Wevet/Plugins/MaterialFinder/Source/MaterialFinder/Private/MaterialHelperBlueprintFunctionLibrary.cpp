// Copyright Epic Games, Inc. All Rights Reserved.

#include "MaterialHelperBlueprintFunctionLibrary.h"
#include "Materials/MaterialInstance.h"


void UMaterialHelperBlueprintFunctionLibrary::SetMaterialInstanceStaticSwitchParameterValue(UMaterialInstance* Instance, FName ParameterName, bool Value)
{

#if WITH_EDITOR
	check(Instance);
	FStaticParameterSet StaticParameters = Instance->GetStaticParameters();
	for (FStaticSwitchParameter& SwitchParameter : StaticParameters.StaticSwitchParameters)
	{
		if (SwitchParameter.ParameterInfo.Name == ParameterName)
		{
			SwitchParameter.Value = Value;
			break;;
		}
	}
	Instance->UpdateStaticPermutation(StaticParameters);
#endif

}


