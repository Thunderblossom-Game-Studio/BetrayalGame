// Fill out your copyright notice in the Description page of Project Settings.

#include "Menu_PlayerController.h"

#include "BetrayalGameNetworkSubsystem.h"
#include "../StaticUtils.h"
#include "OnlineSubsystem.h"
#include "OnlineSubsystemTypes.h"
#include "VoiceChatSubsystem.h"
#include "BetrayalGame/BetrayalPlayerState.h"
#include "Components/EditableText.h"
#include "Components/PanelWidget.h"
#include "Components/TextBlock.h"
#include "GameFramework/GameStateBase.h"
#include "Interfaces/OnlineIdentityInterface.h"

void AMenu_PlayerController::BeginPlay()
{
	Super::BeginPlay();
}

void AMenu_PlayerController::Init()
{
	// Create password field widget
	if (!WB_PasswordField && WB_PasswordFieldClass)
	{
		Print("Creating password field widget");
		WB_PasswordField = CreateWidget<UUserWidget>(GetWorld(), WB_PasswordFieldClass);
	}
	else
	{
		if (WB_PasswordField)
			Print("AMenu_PlayerController::AMenu_PlayerController(): WB_PasswordField already exists!");
		if (!WB_PasswordFieldClass)
			Print("AMenu_PlayerController::AMenu_PlayerController(): WB_PasswordFieldClass is null!");
	}

	// Create Main Menu widget
	if (!WB_MainMenu && WB_MainMenuClass)
	{
		Print("Creating main menu widget");
		WB_MainMenu = CreateWidget<UUserWidget>(GetWorld(), WB_MainMenuClass);
	}
	else
	{
		if (WB_MainMenu)
			Print("AMenu_PlayerController::AMenu_PlayerController(): WB_MainMenu already exists!");
		if (!WB_MainMenuClass)
			Print("AMenu_PlayerController::AMenu_PlayerController(): WB_MainMenuClass is null!");
	}

	// Create Lobby widget
	if (!WB_Lobby && WB_LobbyClass)
	{
		Print("Creating lobby widget");
		WB_Lobby = CreateWidget<UUserWidget>(GetWorld(), WB_LobbyClass);
	}
	else
	{
		if (WB_Lobby)
			Print("AMenu_PlayerController::AMenu_PlayerController(): WB_Lobby already exists!");
		if (!WB_LobbyClass)
			Print("AMenu_PlayerController::AMenu_PlayerController(): WB_LobbyClass is null!");
	}

	// Create lobby room widget
	if (!WB_LobbyRoom && WB_LobbyRoomClass)
	{
		Print("Creating lobby room widget");
		WB_LobbyRoom = CreateWidget<UUserWidget>(GetWorld(), WB_LobbyRoomClass);
	}
	else
	{
		if (WB_LobbyRoom)
			Print("AMenu_PlayerController::AMenu_PlayerController(): WB_LobbyRoom already exists!");
		if (!WB_LobbyRoomClass)
			Print("AMenu_PlayerController::AMenu_PlayerController(): WB_LobbyRoomClass is null!");
	}

	Login();
}

void AMenu_PlayerController::Login()
{
	auto OnlineSubsystem = IOnlineSubsystem::Get();
	if (!OnlineSubsystem)
	{
		Print("No OnlineSubsystem found!");
		return;
	}

	auto Identity = OnlineSubsystem->GetIdentityInterface();
	if (!Identity.IsValid())
	{
		Print("No IdentityInterface found!");
		return;
	}


	FUniqueNetIdPtr UserId = Identity->GetUniquePlayerId(0);
	if (UserId && Identity->GetLoginStatus(0) == ELoginStatus::LoggedIn)
	{
		Print("Already logged in!");
		auto User = Identity->GetUserAccount(*UserId);
		if (User.IsValid())
		{
			Print("User: " + User->GetDisplayName());
		}
	}

	LoginDelegateHandle = Identity->AddOnLoginCompleteDelegate_Handle(0,
	                                                                  FOnLoginCompleteDelegate::CreateUObject(
		                                                                  this,
		                                                                  &AMenu_PlayerController::OnLoginCompleted));

	FString AuthType;
	FParse::Value(FCommandLine::Get(), TEXT("AUTH_TYPE="), AuthType);

	if (!AuthType.IsEmpty())
	{
		Print("Logging in with AuthType: " + AuthType);
		if (!Identity->AutoLogin(0))
		{
			Print("Failed to login with AuthType: " + AuthType);
			Identity->ClearOnLoginCompleteDelegate_Handle(0, LoginDelegateHandle);
			LoginDelegateHandle.Reset();
		}
	}
	else
	{
		FOnlineAccountCredentials Credentials("AccountPortal", "", "");

		Print("Logging in with default AuthType");

		if (!Identity->Login(0, Credentials))
		{
			Print("Failed to login with default AuthType");
			Identity->ClearOnLoginCompleteDelegate_Handle(0, LoginDelegateHandle);
			LoginDelegateHandle.Reset();
		}
	}
}

void AMenu_PlayerController::OnLoginCompleted(int32 LocalUserNum, bool bWasSuccessful, const FUniqueNetId& UserId,
                                              const FString& Error)
{
	auto Subsystem = IOnlineSubsystem::Get();
	auto Identity = Subsystem->GetIdentityInterface();

	if (!Subsystem || !Identity)
		return;

	if (bWasSuccessful)
	{
		Print("Login successful!");
		auto User = Identity->GetUserAccount(UserId);
		if (User.IsValid())
		{
			Print("User: " + User->GetDisplayName());
		}
	}
	else
	{
		Print("Login failed: " + Error);
	}

	Identity->ClearOnLoginCompleteDelegate_Handle(LocalUserNum, LoginDelegateHandle);
	LoginDelegateHandle.Reset();

	// Set up the voice chat subsystem
	auto VC = GetGameInstance()->GetSubsystem<UVoiceChatSubsystem>();
	if (VC)
		VC->Init();
}

void AMenu_PlayerController::ShowMainMenu()
{
	// Add the main menu widget to the viewport
	if (WB_MainMenu)
		WB_MainMenu->AddToViewport();
	else
	{
		// Returns out to skip setting focus and binding buttons if the widget is null
		Print("AMenu_PlayerController::ShowMainMenu(): WB_MainMenu is null!");
		return;
	}

	// Set input mode to UI only
	FInputModeUIOnly InputMode;

	// Get the play button and set it as the focus
	if (const UWidget* PlayButton = WB_MainMenu->GetWidgetFromName("Btn_Play"))
		InputMode.SetWidgetToFocus(PlayButton->GetCachedWidget());

	SetInputMode(InputMode);
	bShowMouseCursor = true;
}

void AMenu_PlayerController::HideMainMenu()
{
	// Remove the main menu widget from the viewport
	if (WB_MainMenu)
		WB_MainMenu->RemoveFromParent();
	else
		Print("AMenu_PlayerController::HideMainMenu(): WB_MainMenu is null!");
}

void AMenu_PlayerController::ShowLobby()
{
	// Add the Lobby widget to the viewport
	if (WB_Lobby)
		WB_Lobby->AddToViewport();
	else
	{
		// Returns out to skip setting focus if the widget is null
		Print("AMenu_PlayerController::ShowLobby(): WB_Lobby is null!");
		return;
	}

	// Move input focus to the lobby
	FInputModeUIOnly InputMode;

	if (const UWidget* PlayButton = WB_Lobby->GetWidgetFromName("Btn_HostGame"))
		InputMode.SetWidgetToFocus(PlayButton->GetCachedWidget());
	else
		Print("AMenu_PlayerController::ShowLobby(): Host game button not found!");


	InputMode.SetWidgetToFocus(WB_Lobby->GetCachedWidget());
	SetInputMode(InputMode);
}

void AMenu_PlayerController::HideLobby()
{
	// Remove the Lobby widget from the viewport
	if (WB_Lobby)
		WB_Lobby->RemoveFromParent();
	else
		Print("AMenu_PlayerController::HideLobby(): WB_Lobby is null!");
}

void AMenu_PlayerController::ShowPasswordField()
{
	if (WB_PasswordField)
		WB_PasswordField->AddToViewport();
	else
	{
		Print("AMenu_PlayerController::ShowPasswordField(): WB_PasswordField is null!");
		return;
	}

	// Set focus on the password field
	FInputModeUIOnly InputMode;

	if (UWidget* PasswordField = WB_PasswordField->GetWidgetFromName("Passwd"))
	{
		InputMode.SetWidgetToFocus(PasswordField->GetCachedWidget());
		UEditableText* pw = Cast<UEditableText>(PasswordField);
		pw->SetText(FText::FromString(""));
	}
	else
		Print("AMenu_PlayerController::ShowPasswordField(): Password field not found!");

	SetInputMode(InputMode);
}

void AMenu_PlayerController::HidePasswordField()
{
	if (WB_PasswordField)
	{
		WB_PasswordField->RemoveFromParent();
		UEditableText* pw = Cast<UEditableText>(WB_PasswordField->GetWidgetFromName("Passwd"));
		if (pw) pw->SetText(FText::FromString(""));
	}
	else
		Print("AMenu_PlayerController::HidePasswordField(): WB_PasswordField is null!");
}

void AMenu_PlayerController::ShowLobbyRoom()
{
	if (WB_LobbyRoom)
		WB_LobbyRoom->AddToViewport();
	else
	{
		Print("AMenu_PlayerController::ShowLobbyRoom(): WB_LobbyRoom is null!");
		return;
	}

	// Create a callback to UBetrayalGameNetworkSubsystem::OnClientConnected to update the UI list of connected clients
	UBetrayalGameNetworkSubsystem* NetworkSubsystem = GetGameInstance()->GetSubsystem<UBetrayalGameNetworkSubsystem>();
	if (NetworkSubsystem)
	{
		// Check if the delegate is already bound to avoid multiple bindings		
		//		if (!NetworkSubsystem->OnClientsChangedDelegate.Contains(this, "UpdateReadyStates"))
		{
			NetworkSubsystem->OnClientsChangedDelegate.AddDynamic(this, &ThisClass::UpdateUI);
			Print("AMenu_PlayerController::ShowLobbyRoom(): OnClientsChangedDelegate bound!");
		}
		//		else
		//			Print("AMenu_PlayerController::ShowLobbyRoom(): OnClientsChangedDelegate is already bound!");
	}
	else
		Print("AMenu_PlayerController::ShowLobbyRoom(): NetworkSubsystem is null!");
}

void AMenu_PlayerController::HideLobbyRoom()
{
	if (WB_LobbyRoom)
		WB_LobbyRoom->RemoveFromParent();
	else
		Print("AMenu_PlayerController::HideLobbyRoom(): WB_LobbyRoom is null!");

	// Remove the callback to UBetrayalGameNetworkSubsystem::OnClientConnected
	UBetrayalGameNetworkSubsystem* NetworkSubsystem = GetGameInstance()->GetSubsystem<UBetrayalGameNetworkSubsystem>();
	if (NetworkSubsystem)
	{
		NetworkSubsystem->OnClientsChangedDelegate.RemoveDynamic(this, &ThisClass::UpdateUI);
	}
	else
		Print("AMenu_PlayerController::HideLobbyRoom(): NetworkSubsystem is null!");

	ClearPlayerList();
}

void AMenu_PlayerController::UpdateUI()
{
	Print("Updating UI...");

	FTimerHandle TimerHandle;
	// Update the player list after a delay to allow the network subsystem to update the list of connected clients
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &ThisClass::UpdatePlayerList, 5.0f, false);
	// Update the list of players' ready states so clients can see the current state of the lobby
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &ThisClass::UpdateReadyStates, 5.5f, false);
}

void AMenu_PlayerController::UpdatePlayerList()
{
	if (!WB_LobbyRoom)
	{
		Print("AMenu_PlayerController::UpdatePlayerList(): WB_LobbyRoom is null!");
		return;
	}

	auto PlayerListWidget = WB_LobbyRoom->GetWidgetFromName("PlayerList");
	if (!PlayerListWidget)
	{
		Print("AMenu_PlayerController::UpdatePlayerList(): PlayerListWidget is null!");
		return;
	}

	UPanelWidget* PlayerList = Cast<UPanelWidget>(PlayerListWidget);
	if (!PlayerList)
	{
		Print("AMenu_PlayerController::UpdatePlayerList(): PlayerList is null!");
		return;
	}

	PlayerList->ClearChildren();

	// Iterate over the player states and add each unique user to the list
	auto Plrs = GetWorld()->GetGameState()->PlayerArray;
	for (auto Plr : Plrs)
	{
		Print("Updating player: " + Plr->GetPlayerName());

		if (auto PlayerRef = Cast<ABetrayalPlayerState>(Plr))
		{
			CreatePlayerNameTextWidget(PlayerRef->GetPlayerName(), PlayerRef->IsReady(), PlayerList);

			if (UBetrayalGameNetworkSubsystem* NetworkSubsystem = GetGameInstance()->GetSubsystem<
				UBetrayalGameNetworkSubsystem>())
			{
				//NetworkSubsystem->UpdatePlayerNameText(PlayerNameWidget);
			}
		}
	}

	// Update the start game button to be enabled if the player is the host
	auto StartGameButton = WB_LobbyRoom->GetWidgetFromName("Btn_StartGame");
	if (!StartGameButton)
		return;

	// Check if the player is the host and disable the button if they are not
	if (!HasAuthority())
	{
		StartGameButton->SetIsEnabled(false);
		return;
	}

	// Check if all players are ready
	bool AllPlayersReady = true;
	for (auto Plr : Plrs)
	{
		if (const auto PlayerRef = Cast<ABetrayalPlayerState>(Plr))
		{
			if (!PlayerRef->IsReady())
			{
				AllPlayersReady = false;
				break;
			}
		}
	}

	// Enable the start game button if all players are ready
	StartGameButton->SetIsEnabled(AllPlayersReady);
}

void AMenu_PlayerController::UpdateReadyStates()
{
	// Broadcast an update of current player ready state for the lobby room
	if (const auto PlrState = GetPlayerState<ABetrayalPlayerState>())
	{
		PlrState->SetIsReady(PlrState->IsReady());
	}
}

void AMenu_PlayerController::ClearPlayerList()
{
	if (!WB_LobbyRoom)
	{
		Print("AMenu_PlayerController::ClearPlayerList(): WB_LobbyRoom is null!");
		return;
	}

	const auto PlayerListWidget = WB_LobbyRoom->GetWidgetFromName("PlayerList");
	if (!PlayerListWidget)
	{
		Print("AMenu_PlayerController::ClearPlayerList(): PlayerListWidget is null!");
		return;
	}

	UPanelWidget* PlayerList = Cast<UPanelWidget>(PlayerListWidget);
	if (!PlayerList)
	{
		Print("AMenu_PlayerController::ClearPlayerList(): PlayerList is null!");
		return;
	}

	PlayerList->ClearChildren();
}
