// Copyright Epic Games, Inc. All Rights Reserved.

#include "HauntConfigWindow.h"
#include "HauntConfigWindowStyle.h"
#include "HauntConfigWindowCommands.h"
#include "LevelEditor.h"
#include "SHauntConfigMenu.h"
#include "Widgets/Docking/SDockTab.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Text/STextBlock.h"
#include "ToolMenus.h"

static const FName HauntConfigWindowTabName("HauntConfigWindow");

#define LOCTEXT_NAMESPACE "FHauntConfigWindowModule"

void FHauntConfigWindowModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	
	FHauntConfigWindowStyle::Initialize();
	FHauntConfigWindowStyle::ReloadTextures();

	FHauntConfigWindowCommands::Register();
	
	PluginCommands = MakeShareable(new FUICommandList);

	PluginCommands->MapAction(
		FHauntConfigWindowCommands::Get().OpenPluginWindow,
		FExecuteAction::CreateRaw(this, &FHauntConfigWindowModule::PluginButtonClicked),
		FCanExecuteAction());

	UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FHauntConfigWindowModule::RegisterMenus));
	
	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(HauntConfigWindowTabName, FOnSpawnTab::CreateRaw(this, &FHauntConfigWindowModule::OnSpawnPluginTab))
		.SetDisplayName(LOCTEXT("FHauntConfigWindowTabTitle", "HauntConfigWindow"))
		.SetMenuType(ETabSpawnerMenuType::Hidden);
}

void FHauntConfigWindowModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

	UToolMenus::UnRegisterStartupCallback(this);

	UToolMenus::UnregisterOwner(this);

	FHauntConfigWindowStyle::Shutdown();

	FHauntConfigWindowCommands::Unregister();

	FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(HauntConfigWindowTabName);
}

TSharedRef<SDockTab> FHauntConfigWindowModule::OnSpawnPluginTab(const FSpawnTabArgs& SpawnTabArgs)
{
	FText WidgetText = FText::Format(
		LOCTEXT("WindowWidgetText", "Hello World!"),
		FText::FromString(TEXT("FHauntConfigWindowModule::OnSpawnPluginTab")),
		FText::FromString(TEXT("HauntConfigWindow.cpp"))
		);

	return SNew(SDockTab)
		.TabRole(ETabRole::NomadTab)
		[
			// Put your tab content here!
			SNew(SHauntConfigMenu)
		];
}

void FHauntConfigWindowModule::PluginButtonClicked()
{
	FGlobalTabmanager::Get()->TryInvokeTab(HauntConfigWindowTabName);
}

void FHauntConfigWindowModule::RegisterMenus()
{
	// Owner will be used for cleanup in call to UToolMenus::UnregisterOwner
	FToolMenuOwnerScoped OwnerScoped(this);

	{
		UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("LevelEditor.MainMenu.Window");
		{
			FToolMenuSection& Section = Menu->FindOrAddSection("WindowLayout");
			Section.AddMenuEntryWithCommandList(FHauntConfigWindowCommands::Get().OpenPluginWindow, PluginCommands);
		}
	}

	{
		UToolMenu* ToolbarMenu = UToolMenus::Get()->ExtendMenu("LevelEditor.LevelEditorToolBar");
		{
			FToolMenuSection& Section = ToolbarMenu->FindOrAddSection("Settings");
			{
				FToolMenuEntry& Entry = Section.AddEntry(FToolMenuEntry::InitToolBarButton(FHauntConfigWindowCommands::Get().OpenPluginWindow));
				Entry.SetCommandList(PluginCommands);
			}
		}
	}
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FHauntConfigWindowModule, HauntConfigWindow)