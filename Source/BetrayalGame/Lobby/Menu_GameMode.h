// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "Menu_GameMode.generated.h"

/**
 * 
 */
UCLASS()
class BETRAYALGAME_API AMenu_GameMode : public AGameMode
{
	GENERATED_BODY()

	APlayerController* Login(UPlayer* NewPlayer, ENetRole InRemoteRole, const FString& Portal, const FString& Options, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage) override;
	
};
