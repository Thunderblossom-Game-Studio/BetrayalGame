// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"
#include "HauntConfigWindowStyle.h"

class FHauntConfigWindowCommands : public TCommands<FHauntConfigWindowCommands>
{
public:

	FHauntConfigWindowCommands()
		: TCommands<FHauntConfigWindowCommands>(TEXT("HauntConfigWindow"), NSLOCTEXT("Contexts", "HauntConfigWindow", "HauntConfigWindow Plugin"), NAME_None, FHauntConfigWindowStyle::GetStyleSetName())
	{
	}

	// TCommands<> interface
	virtual void RegisterCommands() override;

public:
	TSharedPtr< FUICommandInfo > OpenPluginWindow;
};