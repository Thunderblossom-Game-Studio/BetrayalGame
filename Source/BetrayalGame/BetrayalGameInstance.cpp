// Fill out your copyright notice in the Description page of Project Settings.

#include "BetrayalGameInstance.h"
#include "StaticUtils.h"
#include "OnlineSessionSettings.h"
#include "Online/OnlineSessionNames.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "Steam/steam_api.h"

#pragma region General

UBetrayalGameInstance::UBetrayalGameInstance(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// Session creation function binding
	OnCreateSessionCompleteDelegate = FOnCreateSessionCompleteDelegate::CreateUObject(
		this, &UBetrayalGameInstance::OnCreateSessionComplete);
	OnStartSessionCompleteDelegate = FOnStartSessionCompleteDelegate::CreateUObject(
		this, &UBetrayalGameInstance::OnCreateSessionComplete);

	// Session search function binding
	OnFindSessionsCompleteDelegate = FOnFindSessionsCompleteDelegate::CreateUObject(
		this, &UBetrayalGameInstance::OnFindSessionsComplete);

	// Session join function binding
	OnJoinSessionCompleteDelegate = FOnJoinSessionCompleteDelegate::CreateUObject(
		this, &UBetrayalGameInstance::OnJoinSessionComplete);

	// Session destroy function binding
	OnDestroySessionCompleteDelegate = FOnDestroySessionCompleteDelegate::CreateUObject(
		this, &UBetrayalGameInstance::OnDestroySessionComplete);
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
	// Create the main menu widget if not already created
	if (!WB_MainMenu)
	{
		WB_MainMenu = CreateWidget<UUserWidget>(GetWorld(), WB_MainMenuClass);
	}

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

	// Bind the play button to the play function
	if (UButton* PlayButton = Cast<UButton>(WB_MainMenu->GetWidgetFromName("Btn_Play")))
	{
		//PlayButton->OnClicked.AddDynamic(this, &UBetrayalGameInstance::ShowLobby);
		//PlayButton->OnClicked.AddDynamic(this, &UBetrayalGameInstance::HideMainMenu);
	}
	else
		Print("UBetrayalGameInstance::ShowMainMenu(): Play button not found!");

	// Bind the quit button to the quit function
	// if (UButton* QuitButton = Cast<UButton>(WB_MainMenu->GetWidgetFromName("Btn_Quit")))
	// 	//QuitButton->OnClicked.AddDynamic(this, &UBetrayalGameInstance::QuitGame);
	// else
	// 	Print("UBetrayalGameInstance::ShowMainMenu(): Quit button not found!");
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
	// Create the Lobby widget if not already created
	if (!WB_Lobby && WB_LobbyClass)
		WB_Lobby = CreateWidget<UUserWidget>(GetWorld(), WB_LobbyClass);

	// Add the Lobby widget to the viewport
	if (WB_Lobby)
		WB_Lobby->AddToViewport();

	if (!WB_Lobby)
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
	else
		Print("UBetrayalGameInstance::ShowLobby(): Player controller not found!");

	// Set up button bindings
	if (UButton* HostButton = Cast<UButton>(WB_Lobby->GetWidgetFromName("Btn_HostGame")))
	{
		//HostButton->OnClicked.AddDynamic(this, &UBetrayalGameInstance::UI_HostGame);
		//HostButton->OnClicked.AddDynamic(this, &UBetrayalGameInstance::HideLobby);
	}
	else
		Print("UBetrayalGameInstance::ShowLobby(): Host game button not found!");

	if (UButton* JoinButton = Cast<UButton>(WB_Lobby->GetWidgetFromName("Btn_JoinGame")))
	{
		//JoinButton->OnClicked.AddDynamic(this, &UBetrayalGameInstance::UI_JoinGame);
		//JoinButton->OnClicked.AddDynamic(this, &UBetrayalGameInstance::HideLobby);

		// Search for online games
	}
	else
		Print("UBetrayalGameInstance::ShowLobby(): Join game button not found!");
}

void UBetrayalGameInstance::HideLobby()
{
	// Remove the Lobby widget from the viewport
	if (WB_Lobby)
		WB_Lobby->RemoveFromParent();
	else
		Print("UBetrayalGameInstance::HideLobby(): WB_Lobby is null!");
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
const TSharedPtr<const FUniqueNetId> UBetrayalGameInstance::GetNetID()
{
	return UGameplayStatics::GetGameInstance(GetWorld())->GetFirstGamePlayer()->GetPreferredUniqueNetId().GetUniqueNetId();
}

void UBetrayalGameInstance::UI_HostGame()
{
	HostSession(GetNetID(), NAME_GameSession, false, true, 6);
}

void UBetrayalGameInstance::UI_JoinGame()
{
	FindOnlineGames(false, false);
}

bool UBetrayalGameInstance::HostSession(TSharedPtr<const FUniqueNetId> UserId, FName SessionName, bool bIsLAN,
                                        bool bIsPresence, int32 MaxNumPlayers)
{
	IOnlineSubsystem* const OnlineSubsystem = IOnlineSubsystem::Get();

	if (!OnlineSubsystem)
	{
		Print("UBetrayalGameInstance::HostSession(): No Online Subsystem found!");
		return false;
	}

	IOnlineSessionPtr Sessions = OnlineSubsystem->GetSessionInterface();

	if (!Sessions.IsValid())
	{
		Print("UBetrayalGameInstance::HostSession(): No Session Interface found!");
		return false;
	}

	if (Sessions.IsValid() && UserId.IsValid())
	{
		SessionSettings = MakeShareable(new FOnlineSessionSettings());

		SessionSettings->bIsLANMatch = bIsLAN;
		SessionSettings->bUsesPresence = bIsPresence;
		SessionSettings->NumPublicConnections = MaxNumPlayers;
		SessionSettings->NumPrivateConnections = 0;
		SessionSettings->bAllowInvites = true;
		SessionSettings->bAllowJoinInProgress = true;
		SessionSettings->bShouldAdvertise = true;
		SessionSettings->bAllowJoinViaPresence = true;
		SessionSettings->bAllowJoinViaPresenceFriendsOnly = false;
		SessionSettings->bUseLobbiesIfAvailable = true;

		if(LevelToLoad != "")
		{
			const FString MapName = LevelToLoad;
			SessionSettings->Set(SETTING_MAPNAME, MapName, EOnlineDataAdvertisementType::ViaOnlineService);
			UGameplayStatics::OpenLevel(GetWorld(), FName(*MapName), true);
		}
		else
		{
			Print("UBetrayalGameInstance::HostSession(): LevelToLoad is empty!");
		}

		// Set the delegate to the handle of the session created function
		CreateSessionDelegateHandle = Sessions->AddOnCreateSessionCompleteDelegate_Handle(OnCreateSessionCompleteDelegate);

		// Create the session
		return Sessions->CreateSession(*UserId, SessionName, *SessionSettings);
	}
	else
	{
		Print("No Online Subsystem found!");
	}
	return false;
}

void UBetrayalGameInstance::OnCreateSessionComplete(FName SessionName, bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
		Print("Successfully created session: " + SessionName.ToString());
	}
	else
	{
		Print("Failed to create session: " + SessionName.ToString());
	}

	IOnlineSubsystem* const OnlineSubsystem = IOnlineSubsystem::Get();
	if (!OnlineSubsystem)
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("No Online Subsystem found!")));
		return;
	}

	IOnlineSessionPtr Sessions = OnlineSubsystem->GetSessionInterface();
	if (!Sessions.IsValid())
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("No Session Interface found!")));
		return;
	}

	Sessions->ClearOnCreateSessionCompleteDelegate_Handle(CreateSessionDelegateHandle);

	if (bWasSuccessful)
	{
		// Set the delegate to the handle of the session start function
		StartSessionDelegateHandle = Sessions->AddOnStartSessionCompleteDelegate_Handle(
			OnStartSessionCompleteDelegate);

		// Start the session
		Sessions->StartSession(SessionName);

		// ServerTravel to the lobby
		UGameplayStatics::OpenLevel(GetWorld(), FName(*LevelToLoad), true, "listen");
	}
}

void UBetrayalGameInstance::OnStartOnlineGameComplete(FName SessionName, bool bWasSuccessful)
{
	Print("Session started: " + SessionName.ToString() + ", " + FString::FromInt(bWasSuccessful));

	IOnlineSubsystem* const OnlineSubsystem = IOnlineSubsystem::Get();
	if (!OnlineSubsystem)
	{
		Print("No Online Subsystem found!");
		return;
	}

	IOnlineSessionPtr Sessions = OnlineSubsystem->GetSessionInterface();
	if (!Sessions.IsValid())
	{
		Print("No Session Interface found!");
		return;
	}

	Sessions->ClearOnStartSessionCompleteDelegate_Handle(StartSessionDelegateHandle);

	if (bWasSuccessful)
	{
		// Travel to the lobby
		UGameplayStatics::OpenLevel(GetWorld(), "L_Map", true, "listen");
	}
}

void UBetrayalGameInstance::FindSessions(TSharedPtr<const FUniqueNetId> UserId, bool bIsLAN, bool bIsPresence)
{
	// Get the Online Subsystem
	IOnlineSubsystem* const OnlineSubsystem = IOnlineSubsystem::Get();
	if (!OnlineSubsystem)
	{
		Print("No Online Subsystem found!");
		return;
	}

	IOnlineSessionPtr Sessions = OnlineSubsystem->GetSessionInterface();

	if (Sessions.IsValid() && UserId.IsValid())
	{
		// Build session search parameters
		SessionSearch = MakeShareable(new FOnlineSessionSearch());

		SessionSearch->bIsLanQuery = bIsLAN;
		SessionSearch->MaxSearchResults = 10000;
		SessionSearch->PingBucketSize = 5000;
		SessionSearch->QuerySettings.Set(SEARCH_PRESENCE, bIsPresence, EOnlineComparisonOp::Equals);

		TSharedRef<FOnlineSessionSearch> SearchSettingsRef = SessionSearch.ToSharedRef();

		OnFindSessionsCompleteDelegateHandle = Sessions->AddOnFindSessionsCompleteDelegate_Handle(
			OnFindSessionsCompleteDelegate);

		Sessions->FindSessions(*UserId, SearchSettingsRef);
	}
	else
	{
		Print("No Session Interface found!");
		OnFindSessionsComplete(false);
	}
}

void UBetrayalGameInstance::OnFindSessionsComplete(bool bWasSuccessful)
{
	const FString log = (bWasSuccessful) ? "Session search completed successfully!" : "Session search failed!";
	Print(log);

	IOnlineSubsystem* const OnlineSubsystem = IOnlineSubsystem::Get();
	if (!OnlineSubsystem)
	{
		Print("No Online Subsystem found!");
		return;
	}

	IOnlineSessionPtr Sessions = OnlineSubsystem->GetSessionInterface();
	if (!Sessions.IsValid())
	{
		Print("No Session Interface found!");
		return;
	}

	Sessions->ClearOnFindSessionsCompleteDelegate_Handle(OnFindSessionsCompleteDelegateHandle);

	// Log number of results
	Print("Found results: " + FString::FromInt(SessionSearch->SearchResults.Num()));

	// // Log info about each session
	// for (int32 i = 0; i < SessionSearch->SearchResults.Num(); i++)
	// {
	// 	Print("Session " + FString::FromInt(i) + ": " + SessionSearch->SearchResults[i].Session.OwningUserName);
	// }

	// Create a widget for each session found
	if(!WB_Lobby)
	{
		Print("UBetrayalGameInstance::OnFindSessionsComplete(): WB_Lobby is null!");
		return;
	}

	// Clear the session list
	ClearSessions();

	// Reference to session list
	UWidget* SessionList = WB_Lobby->GetWidgetFromName("Scrl_Sessions");
	if(!SessionList)
	{
		Print("UBetrayalGameInstance::OnFindSessionsComplete(): Session list is null!");
		return;
	}

	// Populate the session list
	for (int32 i = 0; i < SessionSearch->SearchResults.Num(); i++)
	{
		const FString SessionName = SessionSearch->SearchResults[i].Session.OwningUserName;
		const int32 ConnectedPlayers = SessionSearch->SearchResults[i].Session.SessionSettings.NumPublicConnections - SessionSearch->SearchResults[i].Session.NumOpenPublicConnections;
		const int32 MaxPlayers = SessionSearch->SearchResults[i].Session.SessionSettings.NumPublicConnections;
		const int32 Ping = SessionSearch->SearchResults[i].PingInMs;
		const int32 SearchResultsIndex = i;

		AddSessionToList(FName(*SessionName), ConnectedPlayers, MaxPlayers, Ping, SearchResultsIndex);
	}
}

bool UBetrayalGameInstance::JoinSession(TSharedPtr<const FUniqueNetId> UserId, FName SessionName, const FOnlineSessionSearchResult& SearchResult)
{
	bool bSuccessful = false;

	IOnlineSubsystem* const OnlineSubsystem = IOnlineSubsystem::Get();
	if (!OnlineSubsystem)
	{
		Print("No Online Subsystem found!");
		return bSuccessful;
	}

	IOnlineSessionPtr Sessions = OnlineSubsystem->GetSessionInterface();

	if (Sessions.IsValid() && UserId.IsValid())
	{
		OnJoinSessionCompleteDelegateHandle = Sessions->AddOnJoinSessionCompleteDelegate_Handle(
			OnJoinSessionCompleteDelegate);

		bSuccessful = Sessions->JoinSession(*UserId, SessionName, SearchResult);
	}

	return bSuccessful;
}

void UBetrayalGameInstance::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	Print("Session joined: " + SessionName.ToString() + ", " + FString::FromInt(Result));

	IOnlineSubsystem* const OnlineSubsystem = IOnlineSubsystem::Get();
	if (!OnlineSubsystem)
	{
		Print("No Online Subsystem found!");
		return;
	}

	IOnlineSessionPtr Sessions = OnlineSubsystem->GetSessionInterface();
	if (!Sessions.IsValid())
	{
		Print("No Session Interface found!");
		return;
	}

	Sessions->ClearOnJoinSessionCompleteDelegate_Handle(OnJoinSessionCompleteDelegateHandle);

	// Get the first local player controller
	APlayerController* const PlayerController = GetFirstLocalPlayerController();

	// Travel to the lobby
	if (PlayerController)
	{
		FString TravelURL;
		if (Sessions->GetResolvedConnectString(SessionName, TravelURL))
		{
			Print(SessionName.ToString() + " resolved to: " + TravelURL);
			PlayerController->ClientTravel(TravelURL + "?listen", ETravelType::TRAVEL_Absolute);
		}
	}
}

void UBetrayalGameInstance::OnDestroySessionComplete(FName SessionName, bool bWasSuccessful)
{
	Print("OnDestroySessionComplete: " + SessionName.ToString() + ", " + FString::FromInt(bWasSuccessful));

	IOnlineSubsystem* const OnlineSubsystem = IOnlineSubsystem::Get();
	if (!OnlineSubsystem)
	{
		Print("No Online Subsystem found!");
		return;
	}

	IOnlineSessionPtr Sessions = OnlineSubsystem->GetSessionInterface();
	if (!Sessions.IsValid())
	{
		Print("No Session Interface found!");
		return;
	}

	Sessions->ClearOnDestroySessionCompleteDelegate_Handle(OnDestroySessionCompleteDelegateHandle);

	if (bWasSuccessful)
	{
		// Travel back to the main menu
		UGameplayStatics::OpenLevel(GetWorld(), "L_MainMenu", true);
	}
}

void UBetrayalGameInstance::StartOnlineGame(bool bIsLAN, bool bIsPresence, int32 MaxNumPlayers)
{
	HostSession(GetNetID(), NAME_GameSession, bIsLAN, bIsPresence, MaxNumPlayers);
}

void UBetrayalGameInstance::FindOnlineGames(bool bIsLAN, bool bIsPresence)
{
	FindSessions(GetNetID(), bIsLAN, bIsPresence);
}

void UBetrayalGameInstance::JoinOnlineGame(int32 SessionIndex)
{	
	// Validate the session search
	if (SessionSearch.IsValid() && SessionSearch->SearchResults.Num() > 0)
	{
		// Join the session
		JoinSession(GetNetID(), NAME_GameSession, SessionSearch->SearchResults[SessionIndex]);
	}
	else
	{
		Print("UBetrayalGameInstance::JoinOnlineGame(): Session search is invalid or empty!");
	}

/*	Original code:
	// Currently joins the first session found
	// Search result
	FOnlineSessionSearchResult SearchResult;

	if (SessionSearch->SearchResults.Num() > 0)
	{
		for (int32 i = 0; i < SessionSearch->SearchResults.Num(); i++)
		{
			if (SessionSearch->SearchResults[i].Session.OwningUserId != GetNetID())
			{
				SearchResult = SessionSearch->SearchResults[i];

				JoinSession(GetNetID(), NAME_GameSession, SearchResult);
				break;
			}
		}
	}
	*/
}

void UBetrayalGameInstance::DestroySessionAndLeaveGame()
{
	IOnlineSubsystem* const OnlineSubsystem = IOnlineSubsystem::Get();
	if (!OnlineSubsystem)
	{
		Print("No Online Subsystem found!");
		return;
	}

	IOnlineSessionPtr Sessions = OnlineSubsystem->GetSessionInterface();
	if (!Sessions.IsValid())
	{
		Print("No Session Interface found!");
		return;
	}

	Sessions->AddOnDestroySessionCompleteDelegate_Handle(OnDestroySessionCompleteDelegate);
	Sessions->DestroySession(NAME_GameSession);
}

#pragma endregion Networking
