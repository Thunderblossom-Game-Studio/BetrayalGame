// Fill out your copyright notice in the Description page of Project Settings.


#include "Menu_GameMode.h"

APlayerController* AMenu_GameMode::Login(UPlayer* NewPlayer, ENetRole InRemoteRole, const FString& Portal,
	const FString& Options, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage)
{
	// Add self to lobby player list
	


	
	return Super::Login(NewPlayer, InRemoteRole, Portal, Options, UniqueId, ErrorMessage);
}
