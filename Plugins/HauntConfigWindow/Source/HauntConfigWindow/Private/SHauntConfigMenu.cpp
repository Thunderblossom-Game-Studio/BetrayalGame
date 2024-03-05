// Fill out your copyright notice in the Description page of Project Settings.


#include "SHauntConfigMenu.h"

#include "SlateOptMacros.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "KismetCompilerModule.h"
#include "Kismet2/KismetEditorUtilities.h"


BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SHauntConfigMenu::Construct(const FArguments& InArgs)
{
	TSharedRef<SComboButton> CategoryComboButton = SNew(SComboButton)
	.VAlign(VAlign_Center)
	.HAlign(HAlign_Left)
	.ContentPadding(WidgetPadding/2)
	.ButtonContent()
	[
		SNew(STextBlock)
		.Text_Lambda(([this]() {return FText::FromString(CategoryName); }))
	]
	.MenuContent()
	[
		SNew(SVerticalBox)
		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			SNew(SButton)
			.ContentPadding(WidgetPadding/2)
			.Text(FText::FromString("Asymmetric"))
			.OnClicked(this, &SHauntConfigMenu::OnAsymmetricClicked)
		]
		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			SNew(SButton)
			.ContentPadding(WidgetPadding/2)
			.Text(FText::FromString("Hidden Asymmetric"))
			.OnClicked(this, &SHauntConfigMenu::OnHiddenAsymmetricClicked)
		]
		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			SNew(SButton)
			.ContentPadding(WidgetPadding/2)
			.Text(FText::FromString("Free For All"))
			.OnClicked(this, &SHauntConfigMenu::OnFFAClicked)
		]
	];

	// TODO - Joseph: Try neatening up this mess (was learning).
	ChildSlot
	[
		// Create meta data settings
		SNew(SVerticalBox)
		+SVerticalBox::Slot()
		.AutoHeight()
		.VAlign(VAlign_Top)
		.HAlign(HAlign_Fill)
		[
			// Haunt Name
			SNew(SHorizontalBox)
			+SHorizontalBox::Slot()
			.VAlign(VAlign_Center)
			.HAlign(HAlign_Left)
			.Padding(WidgetPadding)
			.AutoWidth()
			[
				SNew(STextBlock)
				.Text(FText::FromString("Haunt Name:"))
			]
			+SHorizontalBox::Slot()
			.VAlign(VAlign_Center)
			.HAlign(HAlign_Left)
			.Padding(WidgetPadding)
			.AutoWidth()
			[
				SNew(SEditableTextBox)
				.Text_Lambda([this]() { return FText::FromString(HauntName); })
				.OnTextChanged_Lambda([this](const FText& NewText) { OnHauntNameChanged(NewText); })
			]

			// Haunt Description
			+SHorizontalBox::Slot()
			.VAlign(VAlign_Center)
			.HAlign(HAlign_Left)
			.Padding(WidgetPadding)
			.AutoWidth()
			[
				SNew(STextBlock)
				.Text(FText::FromString("Description:"))
			]			
			+SHorizontalBox::Slot()
			.VAlign(VAlign_Center)
			.HAlign(HAlign_Left)
			.Padding(WidgetPadding)
			.HAlign(HAlign_Fill)
			[
				SNew(SEditableTextBox)
				.Text_Lambda([this]() { return FText::FromString(HauntDescription); })
				.OnTextChanged_Lambda([this](const FText& NewText) { OnHauntDescriptionChanged(NewText); })
			]
		]

		+SVerticalBox::Slot()
		.AutoHeight()
		.VAlign(VAlign_Top)
		.HAlign(HAlign_Left)
		[
			// Use Haunt Timer
			SNew(SHorizontalBox)
			+SHorizontalBox::Slot()
			.AutoWidth()
			.Padding(WidgetPadding)
			.VAlign(VAlign_Center)
			.HAlign(HAlign_Left)
			[
				SNew(STextBlock)
				.Text(FText::FromString("Use Haunt Time:"))
			]
			+SHorizontalBox::Slot()
			.AutoWidth()
			.Padding(WidgetPadding)
			.VAlign(VAlign_Center)
			.HAlign(HAlign_Left)
			[
				SNew(SCheckBox)
				.IsChecked(bUsesTimer ? ECheckBoxState::Checked : ECheckBoxState::Unchecked)
				.OnCheckStateChanged(FOnCheckStateChanged::CreateSP(this, &SHauntConfigMenu::OnUseTimerCheckboxStateChanged))
			]

			// Haunt Duration
			+SHorizontalBox::Slot()
			.AutoWidth()
			.Padding(WidgetPadding)
			.VAlign(VAlign_Center)
			.HAlign(HAlign_Left)
			[
				SNew(STextBlock)
				.Visibility_Lambda([this]() { return bUsesTimer ? EVisibility::All : EVisibility::Hidden; })
				.Text(FText::FromString("Length (Seconds):"))
			]
			+SHorizontalBox::Slot()
			.AutoWidth()
			.Padding(WidgetPadding)
			.VAlign(VAlign_Center)
			.HAlign(HAlign_Left)
			[
				SNew(SEditableTextBox)
				.Visibility_Lambda([this]() { return bUsesTimer ? EVisibility::All : EVisibility::Hidden; })
				.IsEnabled_Lambda([this]() { return bUsesTimer; })
				.Text_Lambda([this]() { return FText::AsNumber(HauntLength_Seconds); })
				.OnTextChanged_Lambda([this](const FText& NewText) { OnHauntLengthChanged(NewText); })
			]
		]
		
		// Create blueprint settings menu
		+SVerticalBox::Slot()
		.AutoHeight()
		.VAlign(VAlign_Top)
		.HAlign(HAlign_Fill)
		[
			SNew(SVerticalBox)
			+SVerticalBox::Slot()
			.VAlign(VAlign_Top)
			.HAlign(HAlign_Left)
			.Padding(WidgetPadding)
			.AutoHeight()
			[
				CategoryComboButton
			]
			
			+SVerticalBox::Slot()
			.VAlign(VAlign_Center)
			.HAlign(HAlign_Left)
			.Padding(WidgetPadding)
			.AutoHeight()
			[
				SNew(STextBlock)
				.Visibility_Lambda([this]() { return (HauntCategory != Hc_FreeForAll) ? EVisibility::All : EVisibility::Hidden; })
				.Text(FText::FromString("Monsters Option Goes Here"))				
			]
		]

		// Create 'Create' button
		+SVerticalBox::Slot()
		.VAlign(VAlign_Bottom)
		.HAlign(HAlign_Right)
		.Padding(WidgetPadding)
		[
			SNew(SVerticalBox)
			+SVerticalBox::Slot()
			.VAlign(VAlign_Center)
			.VAlign(VAlign_Fill)
			.HAlign(HAlign_Center)
			.HAlign(HAlign_Fill)
			[
				SNew(SButton)
				.VAlign(VAlign_Center)
				.VAlign(VAlign_Fill)
				.HAlign(HAlign_Center)
				.HAlign(HAlign_Fill)
				.Text(FText::FromString("Create Haunt Blueprint"))
				.OnClicked(FOnClicked::CreateSP(this, &SHauntConfigMenu::CreateConfiguredBlueprint))
			]
		]
		
	];
}

FReply SHauntConfigMenu::CreateConfiguredBlueprint()
{
	FString TrimmedName = HauntName;
	TrimmedName.RemoveSpacesInline();
	FString BlueprintPath = "/Game/Blueprints/Haunts/BP_" + TrimmedName;
	const TSubclassOf<UObject> ParentClass = ABaseHaunt::StaticClass();

	// TODO - Joseph: Potentially add forced pascal casing to further enforce content style guide.
	
	if (!FKismetEditorUtilities::CanCreateBlueprintOfClass(ParentClass))
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to create blueprint, parent class is not blueprintable..."));
		return FReply::Handled();
	}
	
	FString BPPath = BlueprintPath;
	uint32 i = 0;
	do
	{
		if (i != 0)
		{
			BPPath = BlueprintPath + "_" + FString::FromInt(i);
			UE_LOG(LogTemp, Log, TEXT("Renaming ABaseHaunt blueprint to '%s'..."), *BlueprintPath);
		}
		i++;
	}
	while (StaticLoadObject(UObject::StaticClass(), nullptr, *BPPath));
	BlueprintPath = BPPath;
	
	UE_LOG(LogTemp, Log, TEXT("Creating UBaseHaunt blueprint at '%s'..."), *BlueprintPath);
	
	UPackage* Package = CreatePackage(*BlueprintPath);
	if (Package == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to create blueprint, failed to create asset package..."));
		return FReply::Handled();		
	}

	UClass* BpClass = nullptr;
	UClass* BpGenClass = nullptr;
	FModuleManager::LoadModuleChecked<IKismetCompilerInterface>("KismetCompiler").GetBlueprintTypesForClass(ParentClass, BpClass, BpGenClass);

	UE_LOG(LogTemp, Warning, TEXT("Blueprint at '%s'..."), *BlueprintPath);
	UBlueprint* Blueprint = FKismetEditorUtilities::CreateBlueprint(ParentClass, Package, *FPaths::GetBaseFilename(BlueprintPath), BPTYPE_Normal, BpClass, BpGenClass);
	if (!Blueprint)
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to create blueprint at '%s'..."), *BlueprintPath);
		return FReply::Handled();
	}
	
	FAssetRegistryModule::AssetCreated(Blueprint);
	bool bIsDirty = Blueprint->MarkPackageDirty();

	ConfigureHauntBlueprint(Cast<ABaseHaunt>(Blueprint->GeneratedClass->ClassDefaultObject));
	
	return FReply::Handled();
}

void SHauntConfigMenu::OnUseTimerCheckboxStateChanged(ECheckBoxState NewState)
{
	bUsesTimer = (NewState == ECheckBoxState::Checked);
}

FReply SHauntConfigMenu::OnHauntLengthChanged(const FText& NewText)
{
	FText Trimmed = FText::TrimPrecedingAndTrailing(NewText);
	if (!Trimmed.IsNumeric())
		return FReply::Handled();
	HauntLength_Seconds = FCString::Atoi(*Trimmed.ToString());
	return FReply::Handled();
}

FReply SHauntConfigMenu::OnHauntNameChanged(const FText& NewText)
{
	HauntName = NewText.ToString();
	return FReply::Handled();
}

FReply SHauntConfigMenu::OnHauntDescriptionChanged(const FText& NewText)
{
	HauntDescription = NewText.ToString();
	return FReply::Handled();
}

FReply SHauntConfigMenu::OnAsymmetricClicked()
{
	HauntCategory = Hc_Asymmetric;
	UpdatedConfigMenu();
	return FReply::Handled();
}

FReply SHauntConfigMenu::OnHiddenAsymmetricClicked()
{
	HauntCategory = Hc_HiddenAsymmetric;
	UpdatedConfigMenu();
	return FReply::Handled();
}

FReply SHauntConfigMenu::OnFFAClicked()
{
	HauntCategory = Hc_FreeForAll;
	UpdatedConfigMenu();
	return FReply::Handled();
}

void SHauntConfigMenu::UpdatedConfigMenu()
{	
	switch (HauntCategory)
	{
	case Hc_Asymmetric:		
		CategoryName = "Asymmetric";
		break;
	case Hc_HiddenAsymmetric:
		CategoryName = "Hidden Asymmetric";
		break;
	case Hc_FreeForAll:
		CategoryName = "Free For All";
		break;
	default:
		break;
	}
}

void SHauntConfigMenu::ConfigureHauntBlueprint(ABaseHaunt* Haunt)
{
	if (!Haunt)
	{		
		UE_LOG(LogTemp, Warning, TEXT("Haunt can't be configured, doesn't exist..."));
		return;
	}

	// TODO - Joseph: Integrate tool with Vasco's UBaseHaunt.

	UE_LOG(LogTemp, Warning, TEXT("Configuring Haunt..."));
	Haunt->ConfigureHaunt(
		FName(*HauntName),
		FText::FromString(HauntDescription),
		HauntCategory,
		bUsesTimer,
		HauntLength_Seconds,
		false
		);
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION
