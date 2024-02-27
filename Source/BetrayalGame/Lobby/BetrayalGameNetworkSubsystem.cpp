// Fill out your copyright notice in the Description page of Project Settings.


#include "BetrayalGameNetworkSubsystem.h"

#include "OnlineSubsystem.h"
#include "BetrayalGame/StaticUtils.h"
#include "Kismet/GameplayStatics.h"
#include "OnlineSessionSettings.h"
#include "Widget_SessionConnectBtn.h"
#include "BetrayalGame/BetrayalGameInstance.h"
#include "BetrayalGame/Gameplay/BetrayalPlayerState.h"
#include "GameFramework/GameSession.h"
#include "Online/OnlineSessionNames.h"

const TSharedPtr<const FUniqueNetId> UBetrayalGameNetworkSubsystem::GetNetID()
{
	return GetGameInstance()->GetFirstGamePlayer()->GetPreferredUniqueNetId().GetUniqueNetId();
}

IOnlineSessionPtr UBetrayalGameNetworkSubsystem::GetSessionInterface()
{
	// Function to grab online session ptr, to clean up code

	auto Subsystem = IOnlineSubsystem::Get();
	if (!Subsystem)
	{
		Print("UBetrayalGameNetworkSubsystem::GetSessionInterface(): No Online Subsystem found!");
		return nullptr;
	}

	if (Subsystem->GetSessionInterface().IsValid())
	{
		return Subsystem->GetSessionInterface();
	}
	else
	{
		Print("UBetrayalGameNetworkSubsystem::GetSessionInterface(): SessionInterface is null!");
		return nullptr;
	}
}

void UBetrayalGameNetworkSubsystem::SetupNotifications()
{
	auto Sessions = GetSessionInterface();

	Print("Setting up notifications...");

	if (Sessions.IsValid())
	{
		Sessions->OnSessionParticipantJoinedDelegates.AddUObject(
			this, &UBetrayalGameNetworkSubsystem::OnClientConnected);
		Sessions->OnSessionParticipantLeftDelegates.AddUObject(
			this, &UBetrayalGameNetworkSubsystem::OnClientDisconnected);
	}
	else
	{
		Print("UBetrayalGameNetworkSubsystem::SetupNotifications(): SessionInterface is null!");
	}
}

void UBetrayalGameNetworkSubsystem::CleanupNotifications()
{
	// Cleans up notification system to unbind delegates
	auto Sessions = GetSessionInterface();

	if (Sessions.IsValid())
	{
		Sessions->OnSessionParticipantJoinedDelegates.RemoveAll(this);
		Sessions->OnSessionParticipantLeftDelegates.RemoveAll(this);
	}
	else
	{
		Print("UBetrayalGameNetworkSubsystem::CleanupNotifications(): SessionInterface is null!");
	}
}

void UBetrayalGameNetworkSubsystem::OnClientConnected(FName SessionName, const FUniqueNetId& ID)
{
	// Print name of connecting player
	Print("Player connected: " + ID.ToString());
}

void UBetrayalGameNetworkSubsystem::OnClientDisconnected(FName SessionName, const FUniqueNetId& ID,
                                                         EOnSessionParticipantLeftReason Reason)
{
	// Print name of disconnecting player
	Print("Player disconnected: " + ID.ToString());
}

void UBetrayalGameNetworkSubsystem::ResetSessionSearch()
{
	// Cleanup all session search results
	for (auto button : FoundSessionButtons)
	{
		button->RemoveFromParent();
	}

	// Clear the list of found session buttons
	if (FoundSessionButtons.Num() > 0)
		FoundSessionButtons.Empty();

	// Clear the session search results
	if (SessionSearch.IsValid() && SessionSearch->SearchResults.Num() > 0)
	{
		SessionSearch->SearchResults.Empty();
		SessionSearch = nullptr;
	}

	if (OnFindSessionsCompleteDelegateHandle.IsValid())
	{
		// Clear the session search complete delegate
		OnFindSessionsCompleteDelegateHandle.Reset();
	}

	if (OnJoinSessionCompleteDelegateHandle.IsValid())
	{
		// Clear the session join complete delegate
		OnJoinSessionCompleteDelegateHandle.Reset();
	}

	if (!_GameInstance->SessionPassword.IsEmpty())
	{
		_GameInstance->SessionPassword = "";
	}

	_GameInstance->HidePasswordField();
}

bool UBetrayalGameNetworkSubsystem::HostSession(TSharedPtr<const FUniqueNetId> UserId, FName SessionName, bool bIsLAN,
                                                bool bIsPresence, int32 MaxNumPlayers, bool bIsPrivate,
                                                FString Password)
{
	IOnlineSessionPtr Sessions = GetSessionInterface();

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

void UBetrayalGameNetworkSubsystem::BP_HostSession(FName SessionName, bool bIsLAN, bool bIsPresence,
                                                   int32 MaxNumPlayers,
                                                   bool bIsPrivate, FString Password)
{
	// TODO: Add session name support
	HostSession(GetNetID(), NAME_GameSession, bIsLAN, bIsPresence, MaxNumPlayers, bIsPrivate, Password);
}

void UBetrayalGameNetworkSubsystem::OnCreateSessionComplete(FName SessionName, bool bWasSuccessful)
{
	IOnlineSessionPtr Sessions = GetSessionInterface();
	Sessions->ClearOnCreateSessionCompleteDelegate_Handle(CreateSessionDelegateHandle);
	CreateSessionDelegateHandle.Reset();

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
	IOnlineSessionPtr Sessions = GetSessionInterface();

	Sessions->ClearOnStartSessionCompleteDelegate_Handle(StartSessionDelegateHandle);
	StartSessionDelegateHandle.Reset();

	if (bWasSuccessful)
	{
		// Travel to the lobby
		Print("Server travelling to map...");

		FName Level = "";
		if (_GameInstance)
			Level = _GameInstance->LevelToLoad;
		else
			Level = "L_Map";

		UGameplayStatics::OpenLevel(GetWorld(), Level, true, "listen");
		SetupNotifications();
	}
}

void UBetrayalGameNetworkSubsystem::FindSessions(TSharedPtr<const FUniqueNetId> UserId, bool bIsLAN, bool bIsPresence)
{
	ResetSessionSearch();

	IOnlineSessionPtr Sessions = GetSessionInterface();

	if (Sessions.IsValid() && UserId.IsValid())
	{
		// Build session search parameters
		SessionSearch = MakeShareable(new FOnlineSessionSearch());

		SessionSearch->bIsLanQuery = bIsLAN;
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

void UBetrayalGameNetworkSubsystem::BP_FindSessions(bool bIsLAN, bool bIsPresence, TEnumAsByte<ESessionSearchResult>& Result)
{
	FindSessions(GetNetID(), bIsLAN, bIsPresence);
}

void UBetrayalGameNetworkSubsystem::OnFindSessionsComplete(bool bWasSuccessful)
{
	IOnlineSessionPtr Sessions = GetSessionInterface();

	Sessions->ClearOnFindSessionsCompleteDelegate_Handle(OnFindSessionsCompleteDelegateHandle);

	// Log number of results
	Print("Found results: " + FString::FromInt(SessionSearch->SearchResults.Num()));

	// Populate the session list
	for (auto result : SessionSearch->SearchResults)
	{
		// Create new button for each session
		auto SessionButton = CreateWidget<UWidget_SessionConnectBtn>(_GameInstance,
		                                                             _GameInstance->WB_SessionConnectBtnClass);

		if (SessionButton)
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

	IOnlineSessionPtr Sessions = GetSessionInterface();

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

	IOnlineSessionPtr Sessions = GetSessionInterface();

	if (Sessions.IsValid() && GetNetID().IsValid())
	{
		OnJoinSessionCompleteDelegateHandle = Sessions->AddOnJoinSessionCompleteDelegate_Handle(
			OnJoinSessionCompleteDelegate);

		bSuccessful = Sessions->JoinSession(*GetNetID(), NAME_GameSession, SearchResult);
	}

	return bSuccessful;
}

void UBetrayalGameNetworkSubsystem::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	IOnlineSessionPtr Sessions = GetSessionInterface();

	Sessions->ClearOnJoinSessionCompleteDelegate_Handle(OnJoinSessionCompleteDelegateHandle);

	// Travel to the lobby
	if (APlayerController* const PlayerController = GetGameInstance()->GetFirstLocalPlayerController())
	{
		FString TravelURL;
		if (Sessions->GetResolvedConnectString(SessionName, TravelURL))
		{
			Print(SessionName.ToString() + " resolved to: " + TravelURL);
			PlayerController->ClientTravel(TravelURL, ETravelType::TRAVEL_Absolute);
			SetupNotifications();
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

	ResetSessionSearch();
}

void UBetrayalGameNetworkSubsystem::OnSessionUserInviteAccepted(const bool bWasSuccesful, const int32 ControllerId,
                                                                TSharedPtr<const FUniqueNetId> UserId,
                                                                const FOnlineSessionSearchResult& InviteResult)
{
	Print("Invite accepted: " + FString::FromInt(bWasSuccesful) + ", " + FString::FromInt(ControllerId) + ", " +
		InviteResult.GetSessionIdStr());
	JoinSession(GetNetID(), NAME_GameSession, InviteResult);
}

void UBetrayalGameNetworkSubsystem::OnDestroySessionComplete(FName SessionName, bool bWasSuccessful)
{
	IOnlineSessionPtr Sessions = GetSessionInterface();

	Sessions->ClearOnDestroySessionCompleteDelegate_Handle(OnDestroySessionCompleteDelegateHandle);

	if (bWasSuccessful)
	{
		// Travel back to the main menu
		UGameplayStatics::OpenLevel(GetWorld(), "L_MainMenu", true);
	}
}

void UBetrayalGameNetworkSubsystem::BP_DestroySession()
{
	IOnlineSessionPtr Sessions = GetSessionInterface();

	if (Sessions.IsValid())
	{
		Print("Destroying session...");

		OnDestroySessionCompleteDelegateHandle = Sessions->AddOnDestroySessionCompleteDelegate_Handle(
			OnDestroySessionCompleteDelegate);
		Sessions->DestroySession(NAME_GameSession);
	}
	else
	{
		Print("UBetrayalGameNetworkSubsystem::BP_DestroySession(): SessionInterface is null!");
	}

	CleanupNotifications();
}

void UBetrayalGameNetworkSubsystem::OnHandleDisconnect(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
}

void UBetrayalGameNetworkSubsystem::Multicast_UpdateReadyState_Implementation(int32 PlayerID, bool bReady)
{
}

void UBetrayalGameNetworkSubsystem::Server_UpdateReadyState_Implementation(bool bReady)
{
	bIsReady = bReady;
	//Multicast_UpdateReadyState(, bReady);
}

void UBetrayalGameNetworkSubsystem::HandleNetworkFailure(UWorld* World, UNetDriver* NetDriver,
                                                         ENetworkFailure::Type FailureType, const FString& ErrorString)
{
	Print("Network failure: " + ErrorString);

	switch (FailureType)
	{
	case ENetworkFailure::ConnectionLost:
		Print("Connection lost!");
		BP_DestroySession();
		break;

	case ENetworkFailure::ConnectionTimeout:
		Print("Connection timeout!");
		BP_DestroySession();
		break;

	default:
		Print("Unhandled network error: " + ErrorString);
		break;
	}
}

void UBetrayalGameNetworkSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	_GameInstance = Cast<UBetrayalGameInstance>(GetGameInstance());


	// Session creation function binding
	OnCreateSessionCompleteDelegate = FOnCreateSessionCompleteDelegate::CreateUObject(
		this, &UBetrayalGameNetworkSubsystem::OnCreateSessionComplete);
	OnStartSessionCompleteDelegate = FOnStartSessionCompleteDelegate::CreateUObject(
		this, &UBetrayalGameNetworkSubsystem::OnStartOnlineGameComplete);

	// Session search function binding
	OnFindSessionsCompleteDelegate = FOnFindSessionsCompleteDelegate::CreateUObject(
		this, &UBetrayalGameNetworkSubsystem::OnFindSessionsComplete);

	// Session join function binding
	OnJoinSessionCompleteDelegate = FOnJoinSessionCompleteDelegate::CreateUObject(
		this, &UBetrayalGameNetworkSubsystem::OnJoinSessionComplete);

	// Session destroy function binding
	OnDestroySessionCompleteDelegate = FOnDestroySessionCompleteDelegate::CreateUObject(
		this, &UBetrayalGameNetworkSubsystem::OnDestroySessionComplete);

	auto SessionInterface = GetSessionInterface();
	if (SessionInterface.IsValid())
	{
		SessionInterface->OnSessionUserInviteAcceptedDelegates.AddUObject(
			this, &UBetrayalGameNetworkSubsystem::OnSessionUserInviteAccepted);
	}
	else
	{
		Print("UBetrayalGameNetworkSubsystem::UBetrayalGameNetworkSubsystem(): SessionInterface is null!");
	}

	_GameInstance->GetEngine()->OnNetworkFailure().AddUObject(this, &ThisClass::HandleNetworkFailure);
}
