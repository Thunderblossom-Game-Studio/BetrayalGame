// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget_LobbyMenu.h"

#include "Menu_PlayerController.h"
#include "BetrayalGame/BetrayalGameInstance.h"

void UWidget_LobbyMenu::NativeConstruct()
{
	Super::NativeConstruct();

	APlayerController* Ctrl = GetGameInstance()->GetFirstLocalPlayerController();
	if (auto Menu = Cast<AMenu_PlayerController>(Ctrl))
	{
		Menu->UpdateReadyStates();
	}
}
