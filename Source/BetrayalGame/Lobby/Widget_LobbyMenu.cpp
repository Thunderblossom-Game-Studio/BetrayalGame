// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget_LobbyMenu.h"

#include "BetrayalGame/BetrayalGameInstance.h"

void UWidget_LobbyMenu::NativeConstruct()
{
	Super::NativeConstruct();

	auto _GameInstance = GetGameInstance<UBetrayalGameInstance>();
	_GameInstance->DelayedUpdatePlayerList();
}
