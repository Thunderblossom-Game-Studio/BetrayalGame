// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "GameFramework/SaveGame.h"
#include "OnlineSessionSettings.h"
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

#pragma region General
	UBetrayalGameInstance(const FObjectInitializer& ObjectInitializer);
	virtual void Init() override;
	
	UFUNCTION(BlueprintCallable, Category = "General")
	void QuitGame();
	
#pragma endregion
	
#pragma region UI
public:
	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<UUserWidget> WB_MainMenuClass;

	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<UUserWidget> WB_LobbyClass;

	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<UUserWidget> WB_PasswordFieldClass;

	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<UUserWidget> WB_SessionConnectBtnClass;
	
	UPROPERTY(BlueprintReadOnly, Category = "UI")
	UUserWidget* WB_MainMenu;
	
	UPROPERTY(BlueprintReadOnly, Category = "UI")
	UUserWidget* WB_Lobby;

	UPROPERTY(BlueprintReadOnly, Category = "UI")
	UUserWidget* WB_PasswordField;

	UFUNCTION(BlueprintCallable, Category = "UI")
	void ShowMainMenu();

	UFUNCTION(BlueprintCallable, Category = "UI")
	void HideMainMenu();

	UFUNCTION(BlueprintCallable, Category = "UI")
	void ShowLobby();

	UFUNCTION(BlueprintCallable, Category = "UI")
	void HideLobby();

	UFUNCTION(BlueprintCallable, Category = "UI")
	void ShowPasswordField();

	UFUNCTION(BlueprintCallable, Category = "UI")
	void HidePasswordField();
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

#pragma region Networking	
public:
	UPROPERTY(EditAnywhere, Category = "Networking")
	FString LevelToLoad = "";

	// Session password value
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Networking")
	FString SessionPassword = ""; // Password for the session
#pragma endregion
	
};
