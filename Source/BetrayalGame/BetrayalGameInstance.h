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

	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<UUserWidget> WB_LobbyRoomClass;
	
	UPROPERTY(BlueprintReadOnly, Category = "UI")
	UUserWidget* WB_MainMenu;
	
	UPROPERTY(BlueprintReadOnly, Category = "UI")
	UUserWidget* WB_Lobby;

	UPROPERTY(BlueprintReadOnly, Category = "UI")
	UUserWidget* WB_LobbyRoom;

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

	UFUNCTION(BlueprintCallable, Category = "UI")
	void ShowLobbyRoom();

	UFUNCTION(BlueprintCallable, Category = "UI")
	void HideLobbyRoom();

	UFUNCTION()
	void DelayedUpdatePlayerList();
	
	UFUNCTION()
	void UpdatePlayerList();

	UFUNCTION()
	void ClearPlayerList();
	

	
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
	FName LevelToLoad = "";

	// Session password value
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Networking")
	FString SessionPassword = ""; // Password for the session

	UFUNCTION(BlueprintImplementableEvent, Category = "Networking")
	void OnSessionSearchComplete();


	// Voice chat
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Networking")
	bool bVoiceChatEnabled = false;

	// Voice chat volume
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Networking")
	float VoiceChatVolume = 1.0f;

	// Voice chat threshold
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Networking")
	float VoiceChatThreshold = 0.0f;

	// Push to talk
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Networking")
	bool bPushToTalk = false;

	// Toggle Mic Active
	UFUNCTION(BlueprintCallable, Category = "Networking")
	void StartPTT();

	UFUNCTION(BlueprintCallable, Category = "Networking")
	void StopPTT();

#pragma endregion
	
};
