// Fill out your copyright notice in the Description page of Project Settings.


#include "BetrayalGameNetworkSubsystem.h"

#include "OnlineSubsystem.h"
#include "BetrayalGame/StaticUtils.h"
#include "Kismet/GameplayStatics.h"
#include "OnlineSessionSettings.h"
#include "Widget_SessionConnectBtn.h"
#include "BetrayalGame/BetrayalGameInstance.h"
#include "Online/OnlineSessionNames.h"

const TSharedPtr<const FUniqueNetId> UBetrayalGameNetworkSubsystem::GetNetID()
{
	return GetGameInstance()->GetFirstGamePlayer()->GetPreferredUniqueNetId().GetUniqueNetId();
}

bool UBetrayalGameNetworkSubsystem::HostSession(TSharedPtr<const FUniqueNetId> UserId, FName SessionName, bool bIsLAN,
                                                bool bIsPresence, int32 MaxNumPlayers, bool bIsPrivate,
                                                FString Password)
{
	IOnlineSubsystem* const OnlineSubsystem = IOnlineSubsystem::Get();

	if (!OnlineSubsystem)
	{
		Print("UBetrayalGameNetworkSubsystem::HostSession(): No Online Subsystem found!");
		return false;
	}

	IOnlineSessionPtr Sessions = OnlineSubsystem->GetSessionInterface();

	if (!Sessions.IsValid())
	{
		Print("UBetrayalGameNetworkSubsystem::HostSession(): No Session Interface found!");
		return false;
	}

	if (Sessions.IsValid() && UserId.IsValid())
	{
		SessionSettings = MakeShareable(new FOnlineSessionSettings());

		SessionSettings->bIsLANMatch = bIsLAN;
		SessionSettings->bUsesPresence = bIsPresence;

		if (bIsPrivate)
		{
			// Set the session as private
			SessionSettings->NumPrivateConnections = MaxNumPlayers;
			SessionSettings->NumPublicConnections = 0;
		}
		else
		{
			// Set the session as public
			SessionSettings->NumPrivateConnections = 0;
			SessionSettings->NumPublicConnections = MaxNumPlayers;
		}

		SessionSettings->bAllowInvites = true;
		SessionSettings->bAllowJoinInProgress = true;
		SessionSettings->bShouldAdvertise = true;
		SessionSettings->bAllowJoinViaPresence = true;
		SessionSettings->bAllowJoinViaPresenceFriendsOnly = false;
		SessionSettings->bUseLobbiesIfAvailable = true;

		// Session password
		SessionSettings->Set(PASSWORD, Password, EOnlineDataAdvertisementType::ViaOnlineService);

		// Session server list name
		SessionSettings->Set(SERVERLIST_NAME, LobbyListName, EOnlineDataAdvertisementType::ViaOnlineService);

		if (!_GameInstance->LevelToLoad.IsEmpty())
		{
			const FString MapName = _GameInstance->LevelToLoad;
			SessionSettings->Set(SETTING_MAPNAME, MapName, EOnlineDataAdvertisementType::ViaOnlineService);
			//UGameplayStatics::OpenLevel(GetWorld(), FName(*MapName), true);
			GetWorld()->ServerTravel(MapName + "?listen", true, false);
		}
		else
		{
			Print("UBetrayalGameNetworkSubsystem::HostSession(): LevelToLoad is empty!");
		}

		// Set the delegate to the handle of the session created function
		CreateSessionDelegateHandle = Sessions->AddOnCreateSessionCompleteDelegate_Handle(
			OnCreateSessionCompleteDelegate);

		// Create the session
		return Sessions->CreateSession(*UserId, NAME_GameSession, *SessionSettings);
	}
	else
	{
		Print("No Online Subsystem found!");
	}
	return false;
}

void UBetrayalGameNetworkSubsystem::BP_HostSession(FName SessionName, bool bIsLAN, bool bIsPresence, int32 MaxNumPlayers,
	bool bIsPrivate, FString Password)
{
	HostSession(GetNetID(), NAME_GameSession, bIsLAN, bIsPresence, MaxNumPlayers, bIsPrivate, Password);
}

void UBetrayalGameNetworkSubsystem::OnCreateSessionComplete(FName SessionName, bool bWasSuccessful)
{
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
		Print("Session started successfully!");
		
		// Set the delegate to the handle of the session start function
		StartSessionDelegateHandle = Sessions->AddOnStartSessionCompleteDelegate_Handle(OnStartSessionCompleteDelegate);

		// Start the session
		Sessions->StartSession(NAME_GameSession);
	}
	else
	{
		Print("Failed to create session!");
	}
}

void UBetrayalGameNetworkSubsystem::OnStartOnlineGameComplete(FName SessionName, bool bWasSuccessful)
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
		Print("Server travelling to map...");
		UGameplayStatics::OpenLevel(GetWorld(), "L_Map", true, "listen");
	}
}

void UBetrayalGameNetworkSubsystem::FindSessions(TSharedPtr<const FUniqueNetId> UserId, bool bIsLAN, bool bIsPresence)
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
		//SessionSearch->MaxSearchResults = 10000;
		//SessionSearch->PingBucketSize = 50;
		SessionSearch->QuerySettings.Set(SEARCH_PRESENCE, bIsPresence, EOnlineComparisonOp::Equals);
		SessionSearch->QuerySettings.Set(SEARCH_LOBBIES, true, EOnlineComparisonOp::Equals);

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

void UBetrayalGameNetworkSubsystem::BP_FindSessions(bool bIsLAN, bool bIsPresence)
{
	FindSessions(GetNetID(), bIsLAN, bIsPresence);
}

void UBetrayalGameNetworkSubsystem::OnFindSessionsComplete(bool bWasSuccessful)
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

	// Populate the session list
	for (auto result : SessionSearch->SearchResults)
	{
		// Create new button for each session
		auto SessionButton = CreateWidget<UWidget_SessionConnectBtn>(_GameInstance, _GameInstance->WB_SessionConnectBtnClass);

		if(SessionButton)
		{
			// Set the session data
			SessionButton->SetSessionData(result);
			
			// Add the button to the list of found sessions
			FoundSessionButtons.Add(SessionButton);
		}
		else
		{
			Print("UBetrayalGameNetworkSubsystem::OnFindSessionsComplete(): SessionButton is null!");
		}
	}

	_GameInstance->OnSessionSearchComplete();
}

bool UBetrayalGameNetworkSubsystem::JoinSession(TSharedPtr<const FUniqueNetId> UserId, FName SessionName,
                                                const FOnlineSessionSearchResult& SearchResult)
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
		Print("Attempting to join session: " + SessionName.ToString() + ", " + SearchResult.GetSessionIdStr());
		OnJoinSessionCompleteDelegateHandle = Sessions->AddOnJoinSessionCompleteDelegate_Handle(
			OnJoinSessionCompleteDelegate);

		bSuccessful = Sessions->JoinSession(*UserId, NAME_GameSession, SearchResult);
	}

	return bSuccessful;
}

bool UBetrayalGameNetworkSubsystem::JoinSession(FName SessionName, const FOnlineSessionSearchResult& SearchResult)
{
	bool bSuccessful = false;

	IOnlineSubsystem* const OnlineSubsystem = IOnlineSubsystem::Get();
	if (!OnlineSubsystem)
	{
		Print("No Online Subsystem found!");
		return bSuccessful;
	}

	IOnlineSessionPtr Sessions = OnlineSubsystem->GetSessionInterface();

	if(Sessions.IsValid() && GetNetID().IsValid())
	{
		OnJoinSessionCompleteDelegateHandle = Sessions->AddOnJoinSessionCompleteDelegate_Handle(
			OnJoinSessionCompleteDelegate);

		bSuccessful = Sessions->JoinSession(*GetNetID(), NAME_GameSession, SearchResult);
	}

	return bSuccessful;
}

void UBetrayalGameNetworkSubsystem::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
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

	// Travel to the lobby
	if (APlayerController* const PlayerController = GetGameInstance()->GetFirstLocalPlayerController())
	{
		FString TravelURL;
		if (Sessions->GetResolvedConnectString(SessionName, TravelURL))
		{
			Print(SessionName.ToString() + " resolved to: " + TravelURL);
			PlayerController->ClientTravel(TravelURL, ETravelType::TRAVEL_Absolute);
		}
		else
		{
			Print("Failed to resolve connect string!");
		}
	}
	else
	{
		Print("No Player Controller found!");
	}
}

void UBetrayalGameNetworkSubsystem::OnSessionUserInviteAccepted(const bool bWasSuccesful, const int32 ControllerId,
	TSharedPtr<const FUniqueNetId> UserId, const FOnlineSessionSearchResult& InviteResult)
{
	Print("Invite accepted: " + FString::FromInt(bWasSuccesful) + ", " + FString::FromInt(ControllerId) + ", " + InviteResult.GetSessionIdStr());
	JoinSession(GetNetID(), NAME_GameSession, InviteResult);
}

void UBetrayalGameNetworkSubsystem::OnDestroySessionComplete(FName SessionName, bool bWasSuccessful)
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

void UBetrayalGameNetworkSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	
	// Session creation function binding
	OnCreateSessionCompleteDelegate = FOnCreateSessionCompleteDelegate::CreateUObject(
		this, &UBetrayalGameNetworkSubsystem::OnCreateSessionComplete);
	OnStartSessionCompleteDelegate = FOnStartSessionCompleteDelegate::CreateUObject(
		this, &UBetrayalGameNetworkSubsystem::OnCreateSessionComplete);

	// Session search function binding
	OnFindSessionsCompleteDelegate = FOnFindSessionsCompleteDelegate::CreateUObject(
		this, &UBetrayalGameNetworkSubsystem::OnFindSessionsComplete);

	// Session join function binding
	OnJoinSessionCompleteDelegate = FOnJoinSessionCompleteDelegate::CreateUObject(
		this, &UBetrayalGameNetworkSubsystem::OnJoinSessionComplete);

	// Session destroy function binding
	OnDestroySessionCompleteDelegate = FOnDestroySessionCompleteDelegate::CreateUObject(
		this, &UBetrayalGameNetworkSubsystem::OnDestroySessionComplete);

	// Invite accepted function binding
	OnSessionUserInviteAcceptedDelegate = FOnSessionUserInviteAcceptedDelegate::CreateUObject(
		this, &UBetrayalGameNetworkSubsystem::OnSessionUserInviteAccepted);
	
	// Get the game instance
	_GameInstance = Cast<UBetrayalGameInstance>(GetGameInstance());
	if(!_GameInstance)
	{
		Print("UBetrayalGameNetworkSubsystem::UBetrayalGameNetworkSubsystem(): Game instance is null!");
	}

	const auto SessionInterface = IOnlineSubsystem::Get()->GetSessionInterface();
	if (SessionInterface.IsValid())
	{
		SessionInterface->OnSessionUserInviteAcceptedDelegates.AddUObject(this, &UBetrayalGameNetworkSubsystem::OnSessionUserInviteAccepted);
	}
	else
	{
		Print("UBetrayalGameNetworkSubsystem::UBetrayalGameNetworkSubsystem(): SessionInterface is null!");
	}
}
