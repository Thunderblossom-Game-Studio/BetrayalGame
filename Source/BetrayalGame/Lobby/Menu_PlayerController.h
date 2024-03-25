// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/PanelWidget.h"
#include "GameFramework/PlayerController.h"
#include "Menu_PlayerController.generated.h"

/**
 * 
 */
UCLASS()
class BETRAYALGAME_API AMenu_PlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;
	FDelegateHandle LoginDelegateHandle;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Networking")
	FString SessionPassword = ""; // Password for the session

	UFUNCTION(BlueprintCallable, Category = "Controller")
	void Init();

	void Login();
	void OnLoginCompleted(int32 LocalUserNum, bool bWasSuccessful, const FUniqueNetId& UserId, const FString& Error);

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

	UPROPERTY(EditAnywhere, Category = "UI|PlayerList")
	TSubclassOf<UUserWidget> WB_PlayerNameTextClass;

	UPROPERTY(BlueprintReadWrite, Category = "UI")
	UUserWidget* WB_MainMenu;

	UPROPERTY(BlueprintReadWrite, Category = "UI")
	UUserWidget* WB_Lobby;

	UPROPERTY(BlueprintReadWrite, Category = "UI")
	UUserWidget* WB_LobbyRoom;

	UPROPERTY(BlueprintReadWrite, Category = "UI")
	UUserWidget* WB_PasswordField;

	UFUNCTION(BlueprintImplementableEvent, Category = "UI")
	void ShowMainMenu();

	UFUNCTION(BlueprintImplementableEvent, Category = "UI")
	void HideMainMenu();

	UFUNCTION(BlueprintImplementableEvent, Category = "UI")
	void ShowLobby();

	UFUNCTION(BlueprintImplementableEvent, Category = "UI")
	void HideLobby();

	UFUNCTION(BlueprintImplementableEvent, Category = "UI")
	void ShowPasswordField();

	UFUNCTION(BlueprintImplementableEvent, Category = "UI")
	void HidePasswordField();

	UFUNCTION(BlueprintImplementableEvent, Category = "UI")
	void ShowLobbyRoom();

	UFUNCTION(BlueprintImplementableEvent, Category = "UI")
	void HideLobbyRoom();

	UFUNCTION(BlueprintCallable, Category = "UI")
	void UpdateUI();

	UFUNCTION(BlueprintCallable, Category = "UI")
	void UpdatePlayerList();

	UFUNCTION(BlueprintCallable, Category = "UI")
	void UpdateReadyStates();

	UFUNCTION(BlueprintCallable, Category = "UI")
	void ClearPlayerList();

	UFUNCTION(BlueprintImplementableEvent, Category = "UI")
	void CreatePlayerNameTextWidget(const FString& PlayerName, bool bIsReady, UPanelWidget* ParentWidget);

	UFUNCTION(BlueprintCallable, Category = "UI")
	UUserWidget* GetMainMenuWidget() { return WB_MainMenu; }

	UFUNCTION(BlueprintCallable, Category = "UI")
	UUserWidget* GetLobbyWidget() { return WB_Lobby; }

	UFUNCTION(BlueprintCallable, Category = "UI")
	UUserWidget* GetLobbyRoomWidget() { return WB_LobbyRoom; }

	UFUNCTION(BlueprintCallable, Category = "UI")
	UUserWidget* GetPasswordFieldWidget() { return WB_PasswordField; }
};
