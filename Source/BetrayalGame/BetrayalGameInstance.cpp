// Fill out your copyright notice in the Description page of Project Settings.

#include "BetrayalGameInstance.h"
#include "Blueprint/UserWidget.h"
#include "Components/EditableText.h"
#include "Net/Core/Connection/NetEnums.h"
#include "OnlineSubsystem.h"
#include "IOnlineSubsystemEOS.h"
#include "StaticUtils.h"
#include "Blueprint/UserWidget.h"
#include "Components/EditableText.h"
#include "Components/PanelWidget.h"
#include "Components/TextBlock.h"
#include "GameFramework/GameStateBase.h"
#include "GameFramework/PlayerState.h"
#include "Interfaces/VoiceInterface.h"
#include "Lobby/BetrayalGameNetworkSubsystem.h"

#pragma region General

UBetrayalGameInstance::UBetrayalGameInstance(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{}

void UBetrayalGameInstance::Init()
{
	Super::Init();

	// Create password field widget
	if (!WB_PasswordField && WB_PasswordFieldClass)
		WB_PasswordField = CreateWidget<UUserWidget>(GetWorld(), WB_PasswordFieldClass);

	// Create Main Menu widget
	if (!WB_MainMenu && WB_MainMenuClass)
	{
		WB_MainMenu = CreateWidget<UUserWidget>(GetWorld(), WB_MainMenuClass);
	}

	// Create Lobby widget
	if (!WB_Lobby && WB_LobbyClass)
	{
		WB_Lobby = CreateWidget<UUserWidget>(GetWorld(), WB_LobbyClass);
	}

	// Create lobby room widget
	if (!WB_LobbyRoom && WB_LobbyRoomClass)
	{
		WB_LobbyRoom = CreateWidget<UUserWidget>(GetWorld(), WB_LobbyRoomClass);
	}
}

void UBetrayalGameInstance::QuitGame()
{
	// Quit the game
	GetWorld()->GetFirstPlayerController()->ConsoleCommand("quit");
}


#pragma endregion General

#pragma region UI
void UBetrayalGameInstance::ShowMainMenu()
{
	// Add the main menu widget to the viewport
	if (WB_MainMenu)
		WB_MainMenu->AddToViewport();
	else
	{
		// Returns out to skip setting focus and binding buttons if the widget is null
		Print("UBetrayalGameInstance::ShowMainMenu(): WB_MainMenu is null!");
		return;
	}

	// Set input mode to UI only
	if (APlayerController* PlayerController = GetFirstLocalPlayerController())
	{
		FInputModeUIOnly InputMode;

		// Get the play button and set it as the focus
		if (const UWidget* PlayButton = WB_MainMenu->GetWidgetFromName("Btn_Play"))
			InputMode.SetWidgetToFocus(PlayButton->GetCachedWidget());

		PlayerController->SetInputMode(InputMode);
		PlayerController->bShowMouseCursor = true;
	}
	else
	{
		Print("UBetrayalGameInstance::ShowMainMenu(): Player controller not found!");
	}
}

void UBetrayalGameInstance::HideMainMenu()
{
	// Remove the main menu widget from the viewport
	if (WB_MainMenu)
		WB_MainMenu->RemoveFromParent();
	else
		Print("UBetrayalGameInstance::HideMainMenu(): WB_MainMenu is null!");
}

void UBetrayalGameInstance::ShowLobby()
{
	// Add the Lobby widget to the viewport
	if (WB_Lobby)
		WB_Lobby->AddToViewport();
	else
	{
		// Returns out to skip setting focus if the widget is null
		Print("UBetrayalGameInstance::ShowLobby(): WB_Lobby is null!");
		return;
	}

	// Move input focus to the lobby
	if (APlayerController* PlayerController = GetFirstLocalPlayerController())
	{
		FInputModeUIOnly InputMode;

		if (const UWidget* PlayButton = WB_Lobby->GetWidgetFromName("Btn_HostGame"))
			InputMode.SetWidgetToFocus(PlayButton->GetCachedWidget());
		else
			Print("UBetrayalGameInstance::ShowLobby(): Host game button not found!");


		InputMode.SetWidgetToFocus(WB_Lobby->GetCachedWidget());
		PlayerController->SetInputMode(InputMode);
	}
}

void UBetrayalGameInstance::HideLobby()
{
	// Remove the Lobby widget from the viewport
	if (WB_Lobby)
		WB_Lobby->RemoveFromParent();
	else
		Print("UBetrayalGameInstance::HideLobby(): WB_Lobby is null!");
}

void UBetrayalGameInstance::ShowPasswordField()
{
	if(WB_PasswordField)
		WB_PasswordField->AddToViewport();
	else
	{
		Print("UBetrayalGameInstance::ShowPasswordField(): WB_PasswordField is null!");
		return;
	}

	// Set focus on the password field
	if (APlayerController* PlayerController = GetFirstLocalPlayerController())
	{
		FInputModeUIOnly InputMode;

		if (UWidget* PasswordField = WB_PasswordField->GetWidgetFromName("Passwd"))
		{
			InputMode.SetWidgetToFocus(PasswordField->GetCachedWidget());
			UEditableText* pw = Cast<UEditableText>(PasswordField);
			pw->SetText(FText::FromString(""));
		}
		else
			Print("UBetrayalGameInstance::ShowPasswordField(): Password field not found!");

		PlayerController->SetInputMode(InputMode);
	}
}

void UBetrayalGameInstance::HidePasswordField()
{
	if (WB_PasswordField)
	{
		WB_PasswordField->RemoveFromParent();
		UEditableText* pw = Cast<UEditableText>(WB_PasswordField->GetWidgetFromName("Passwd"));
		if(pw) pw->SetText(FText::FromString(""));
	}
	else
		Print("UBetrayalGameInstance::HidePasswordField(): WB_PasswordField is null!");
}

void UBetrayalGameInstance::ShowLobbyRoom()
{
	if (WB_LobbyRoom)
		WB_LobbyRoom->AddToViewport();
	else
	{
		Print("UBetrayalGameInstance::ShowLobbyRoom(): WB_LobbyRoom is null!");
		return;
	}
}

void UBetrayalGameInstance::HideLobbyRoom()
{
	if (WB_LobbyRoom)
		WB_LobbyRoom->RemoveFromParent();
	else
		Print("UBetrayalGameInstance::HideLobbyRoom(): WB_LobbyRoom is null!");
}

#pragma endregion UI

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