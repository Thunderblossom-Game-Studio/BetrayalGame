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
	PrintLog("AMenu_PlayerController::Init()");
	Login();
}

// TODO: Set up multiple login attempts in case accidental closure or other issues
void AMenu_PlayerController::Login()
{
	if (LoginAttempts >= 3)
	{
		PrintLog("Failed to login after 3 attempts!");
		GetWorld()->Exec(GetWorld(), TEXT("quit"));
		
		return;
	}
	
	auto OnlineSubsystem = IOnlineSubsystem::Get();
	if (!OnlineSubsystem)
	{
		PrintLog("No OnlineSubsystem found!");
		return;
	}

	auto Identity = OnlineSubsystem->GetIdentityInterface();
	if (!Identity.IsValid())
	{
		PrintLog("No IdentityInterface found!");
		return;
	}


	FUniqueNetIdPtr UserId = Identity->GetUniquePlayerId(0);
	if (UserId && Identity->GetLoginStatus(0) == ELoginStatus::LoggedIn)
	{
		PrintLog("Already logged in!");
		auto User = Identity->GetUserAccount(*UserId);
		if (User.IsValid())
		{
			PrintLog("User: " + User->GetDisplayName());
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
		PrintLog("Logging in with AuthType: " + AuthType);
		if (!Identity->AutoLogin(0))
		{
			PrintLog("Failed to login with AuthType: " + AuthType);
			Identity->ClearOnLoginCompleteDelegate_Handle(0, LoginDelegateHandle);
			LoginDelegateHandle.Reset();

			//TODO: Jank
			LoginAttempts++;
			Login();
		}
	}
	else
	{
		FOnlineAccountCredentials Credentials("AccountPortal", "", "");

		PrintLog("Logging in with default AuthType");

		if (!Identity->Login(0, Credentials))
		{
			PrintLog("Failed to login with default AuthType");
			Identity->ClearOnLoginCompleteDelegate_Handle(0, LoginDelegateHandle);
			LoginDelegateHandle.Reset();

			// TODO: Jank
			LoginAttempts++;
			Login();
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
		PrintLog("Login successful!");
		auto User = Identity->GetUserAccount(UserId);
		if (User.IsValid())
		{
			PrintLog("User: " + User->GetDisplayName());
		}
	}
	else
	{
		PrintLog("Login failed: " + Error);
	}

	Identity->ClearOnLoginCompleteDelegate_Handle(LocalUserNum, LoginDelegateHandle);
	LoginDelegateHandle.Reset();

	LoginAttempts = 0;

	// TODO: Fix this bs
	// Set up the voice chat subsystem
	//auto VC = GetGameInstance()->GetSubsystem<UVoiceChatSubsystem>();
	//if (VC)
	//	VC->Init();
}

void AMenu_PlayerController::UpdateUI()
{
	PrintLog("Updating UI...");

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
		PrintLog("AMenu_PlayerController::UpdatePlayerList(): WB_LobbyRoom is null!");
		return;
	}

	auto PlayerListWidget = WB_LobbyRoom->GetWidgetFromName("PlayerList");
	if (!PlayerListWidget)
	{
		PrintLog("AMenu_PlayerController::UpdatePlayerList(): PlayerListWidget is null!");
		return;
	}

	UPanelWidget* PlayerList = Cast<UPanelWidget>(PlayerListWidget);
	if (!PlayerList)
	{
		PrintLog("AMenu_PlayerController::UpdatePlayerList(): PlayerList is null!");
		return;
	}

	PlayerList->ClearChildren();

	// Iterate over the player states and add each unique user to the list
	auto Plrs = GetWorld()->GetGameState()->PlayerArray;
	for (auto Plr : Plrs)
	{
		PrintLog("Updating player: " + Plr->GetPlayerName());

		if (auto PlayerRef = Cast<ABetrayalPlayerState>(Plr))
		{
			CreatePlayerNameTextWidget(PlayerRef->GetPlayerName(), PlayerRef->IsReady(), PlayerList);

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
		PrintLog("AMenu_PlayerController::ClearPlayerList(): WB_LobbyRoom is null!");
		return;
	}

	const auto PlayerListWidget = WB_LobbyRoom->GetWidgetFromName("PlayerList");
	if (!PlayerListWidget)
	{
		PrintLog("AMenu_PlayerController::ClearPlayerList(): PlayerListWidget is null!");
		return;
	}

	UPanelWidget* PlayerList = Cast<UPanelWidget>(PlayerListWidget);
	if (!PlayerList)
	{
		PrintLog("AMenu_PlayerController::ClearPlayerList(): PlayerList is null!");
		return;
	}

	PlayerList->ClearChildren();
}
