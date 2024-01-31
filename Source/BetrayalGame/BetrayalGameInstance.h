// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "GameFramework/SaveGame.h"
#include "BetrayalGameInstance.generated.h"

#pragma region Save/Load
USTRUCT(BlueprintType)
struct FPlayerProfile
{
	GENERATED_BODY()
	FString PlayerName;
};
#pragma endregion

/**
 * 
 */
UCLASS()
class BETRAYALGAME_API UBetrayalGameInstance : public UGameInstance
{
	GENERATED_BODY()
private:
#pragma region UI
	UPROPERTY()
	UUserWidget* WB_MainMenu;

	UPROPERTY()
	UUserWidget* WB_Lobby;

public:
	UFUNCTION(BlueprintCallable, Category = "UI")
	void ShowMainMenu();

	UFUNCTION(BlueprintCallable, Category = "UI")
	void ShowLobby();
#pragma endregion

#pragma region Save/Load
private:
	UPROPERTY()
	USaveGame* SG_PlayerProfile;

	UPROPERTY()
	FString SaveSlotName = "UserProfile";

	UPROPERTY()
	FPlayerProfile PlayerProfileData;

public:
	UFUNCTION(BlueprintCallable, Category = "Save/Load")
	void SavePlayerProfile();

	UFUNCTION(BlueprintCallable, Category = "Save/Load")
	void LoadPlayerProfile();

	UFUNCTION(BlueprintCallable, Category = "Save/Load")
	void CheckPlayerProfile();
	
#pragma endregion
	
};
