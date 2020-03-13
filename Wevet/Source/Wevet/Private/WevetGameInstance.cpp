// Copyright © 2018 wevet works All Rights Reserved.


#include "WevetGameInstance.h"
#include "Kismet/GameplayStatics.h"

UWevetGameInstance::UWevetGameInstance()
	: SaveSlot(TEXT("SaveGame")),
	SaveUserIndex(0)
{
	//
}

UWevetGameInstance* UWevetGameInstance::Instance = nullptr;

UWevetGameInstance* UWevetGameInstance::GetInstance()
{
	return Instance;
}

void UWevetGameInstance::Init()
{
	Super::Init();
	Instance = this;
}

void UWevetGameInstance::BeginDestroy()
{
	Instance = nullptr;
	Super::BeginDestroy();
}


void UWevetGameInstance::SetSavingEnabled(bool bEnabled)
{
	bSavingEnabled = bEnabled;
}

bool UWevetGameInstance::WriteSaveGame()
{
	if (bSavingEnabled)
	{
		//return UGameplayStatics::SaveGameToSlot(GetSaveGame(), SaveSlot, SaveUserIndex);
	}
	return false;
}

void UWevetGameInstance::ResetSaveGame(bool& bClearSuccess)
{
	bClearSuccess = UGameplayStatics::DeleteGameInSlot(SaveSlot, SaveUserIndex);
	bool bWasSavingEnabled = bSavingEnabled;
	bSavingEnabled = false;
	bClearSuccess = LoadOrCreateSaveGame();
	bSavingEnabled = bWasSavingEnabled;
}

bool UWevetGameInstance::LoadOrCreateSaveGame()
{
	// Drop reference to old save game, this will GC out
	//CurrentSaveGame = nullptr;

	if (UGameplayStatics::DoesSaveGameExist(SaveSlot, SaveUserIndex) && bSavingEnabled)
	{
	}

	//if (CurrentSaveGame)
	//{
	//	UE_LOG(LogTemp, Warning, TEXT("Already SaveData : %s"), *FString(__FUNCTION__));
	//	return true;
	//}
	//else
	//{
	//	UE_LOG(LogTemp, Error, TEXT("Create SaveData : %s"), *FString(__FUNCTION__));
	//	return false;
	//}
	return false;
}
