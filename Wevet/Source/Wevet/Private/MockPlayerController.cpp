// Fill out your copyright notice in the Description page of Project Settings.

#include "MockPlayerController.h"
#include "Engine.h"
#include "Blueprint/UserWidget.h"


AMockPlayerController::AMockPlayerController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	//
}

void AMockPlayerController::BeginPlay()
{
	if (this->WidgetMainUI)
	{
		this->Widget = CreateWidget<UUserWidget>(this, this->WidgetMainUI);

		if (this->Widget)
		{
			this->Widget->AddToViewport();
		}

	}
}

void AMockPlayerController::Test(FString InString)
{
	if (InString.IsEmpty())
	{
		return;
	}

	FTCHARToUTF8 ToUtf8Converter(InString.GetCharArray().GetData());
	auto Utf8StringSize = ToUtf8Converter.Length();
	auto Utf8String = ToUtf8Converter.Get();

	FUTF8ToTCHAR ToTCharConverter(Utf8String);
	FString OutString(ToTCharConverter.Get());

	for (int i = 0; i < Utf8StringSize; ++i) 
	{
		
	}
	//TCHAR* Chars = *(InString);
	//ANSICHAR* AnsiChars = TCHAR_TO_ANSI(*InString);
	
	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, InString);
	//GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Black, AnsiChars);
	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Yellow, FString::FromInt(InString.Len()));
	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Blue, FString::FromInt(Utf8StringSize));
	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Green, OutString);

}