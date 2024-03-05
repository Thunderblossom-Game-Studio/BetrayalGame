// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

UENUM()
enum ECategory { Asymmetric, HiddenAsymmetric, FreeForAll};

/**
 * 
 */
class HAUNTCONFIGWINDOW_API SHauntConfigMenu : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SHauntConfigMenu)
	{}
	SLATE_END_ARGS()
	
	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs);

public:
	FReply CreateConfiguredBlueprint();
	FReply OnAsymmetricClicked();
	FReply OnHiddenAsymmetricClicked();
	FReply OnFFAClicked();
	void OnUseTimerCheckboxStateChanged(ECheckBoxState NewState);
	FReply OnHauntLengthChanged(const FText& NewText);
	FReply OnHauntNameChanged(const FText& NewText);
	FReply OnHauntDescriptionChanged(const FText& NewText);

protected:
	void UpdatedConfigMenu();
	void ConfigureHauntBlueprint(class UBaseHaunt* Haunt);
	
protected:
	FString CategoryName = "Category";
	FString HauntName = "Example Name";
	FString HauntDescription = "This is an example description.";
	bool bUsesTimer = true;
	int32 HauntLength_Seconds = 120;
	TEnumAsByte<ECategory> HauntCategory = Asymmetric;

	const int WidgetPadding = 15;
};
