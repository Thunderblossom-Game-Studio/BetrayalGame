// Fill out your copyright notice in the Description page of Project Settings.

#include "BetrayalGameInstance.h"

#include "BetrayalPlayerState.h"

UBetrayalGameInstance::UBetrayalGameInstance(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UBetrayalGameInstance::Init()
{
	Super::Init();
}

void UBetrayalGameInstance::QuitGame()
{
	// Quit the game
	GetWorld()->GetFirstPlayerController()->ConsoleCommand("quit");
}

#pragma region Save/Load
void UBetrayalGameInstance::SavePlayerProfile()
{
}

void UBetrayalGameInstance::LoadPlayerProfile()
{
}

void UBetrayalGameInstance::CheckPlayerProfile()
{
}
#pragma endregion Save/Load

#pragma region Networking

#pragma endregion
