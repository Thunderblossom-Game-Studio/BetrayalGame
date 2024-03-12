// Fill out your copyright notice in the Description page of Project Settings.


#include "Menu_GameMode.h"

#include "VoiceChatSubsystem.h"
#include "BetrayalGame/BetrayalGameInstance.h"
#include "../BetrayalPlayerState.h"

void AMenu_GameMode::OnPostLogin(AController* NewPlayer)
{
	Super::OnPostLogin(NewPlayer);

	// Get reference to the player state
	APlayerState* PlayerState = NewPlayer->PlayerState;
	if (!PlayerState)
	{
		UE_LOG(LogTemp, Error, TEXT("AMenu_GameMode::OnPostLogin(): PlayerState is null!"));
		return;
	}

	// Cast player state to BetrayalPlayerState
	ABetrayalPlayerState* BetrayalPlayerState = Cast<ABetrayalPlayerState>(PlayerState);
	if (!BetrayalPlayerState)
	{
		UE_LOG(LogTemp, Error, TEXT("AMenu_GameMode::OnPostLogin(): BetrayalPlayerState is null!"));
		return;
	}

	// Grab player name as string
	FString PlayerName = NewPlayer->PlayerState->GetPlayerName();

	// Log the player name
	UE_LOG(LogTemp, Warning, TEXT("AMenu_GameMode::OnPostLogin(): PlayerName: %s"), *PlayerName);
}

void AMenu_GameMode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);
}
