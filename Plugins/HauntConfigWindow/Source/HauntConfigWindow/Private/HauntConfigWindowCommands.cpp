// Copyright Epic Games, Inc. All Rights Reserved.

#include "HauntConfigWindowCommands.h"

#define LOCTEXT_NAMESPACE "FHauntConfigWindowModule"

void FHauntConfigWindowCommands::RegisterCommands()
{
	UI_COMMAND(OpenPluginWindow, "HauntConfigWindow", "Bring up HauntConfigWindow window", EUserInterfaceActionType::Button, FInputChord());
}

#undef LOCTEXT_NAMESPACE
