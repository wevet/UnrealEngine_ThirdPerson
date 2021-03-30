#pragma once


//#include "SlateBasics.h"

class UCCImportUI;

class SCCImportWindow : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SCCImportWindow)
		: _ImportUI(NULL)
		, _WidgetWindow()
		//, _SourcePath()
		//, _TargetPath()
	{}

	SLATE_ARGUMENT(UCCImportUI*, ImportUI)
	SLATE_ARGUMENT(TSharedPtr<SWindow>, WidgetWindow)
	//	SLATE_ARGUMENT(FText, SourcePath)
		//SLATE_ARGUMENT(FText, TargetPath)
	SLATE_END_ARGS()

public:
	void Construct(const FArguments& InArgs);
private:
	UCCImportUI*	ccImportUI;
	TWeakPtr< SWindow > WidgetWindow;
	TSharedPtr< SButton > ImportButton;
	//FString TargetPath;
	bool isCCAutoSetup;
	bool isHQSkin;
	bool isLWSkin;
	bool isStandardSkin;
	bool isliveLink;
	//TSharedRef<SWidget> GetTestContent() const;
	FString skinType = "Standard";
	ECheckBoxState IsStandardActive() const;
	ECheckBoxState IsHQActive() const;
	ECheckBoxState IsLWActive() const;
	void OnCCLiveLinkChanged(ECheckBoxState InCheckedState);
	void OnCCAutoSetupChanged(ECheckBoxState InCheckedState);
	void OnStandardChanged(ECheckBoxState InCheckedState);
	void OnHQChanged(ECheckBoxState InCheckedState);
	void OnLWChanged(ECheckBoxState InCheckedState);
	TSharedPtr< SCheckBox > Standard_CheckBox;
	TSharedPtr< SCheckBox > HQ_CheckBox;
	TSharedPtr< SCheckBox > LW_CheckBox;
	TSharedPtr< SCheckBox > CCAutoSetup_CheckBox;
	FReply OnCancel();
	FReply OnNext();
    FReply OnMore();
};