// Fill out your copyright notice in the Description page of Project Settings.

#include "StaticUtils.h"
#include "BetrayalGameInstance.h"
#include "OnlineSessionSettings.h"
#include "Online/OnlineSessionNames.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "Kismet/GameplayStatics.h"

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


#pragma endregion

#pragma region UI
void UBetrayalGameInstance::ShowMainMenu()
{
}

void UBetrayalGameInstance::ShowLobby()
{
}
#pragma endregion

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
#pragma endregion

#pragma region Networking
bool UBetrayalGameInstance::HostSession(TSharedPtr<const FUniqueNetId> UserId, FName SessionName, bool bIsLAN,
                                        bool bIsPresence, int32 MaxNumPlayers)
{
	IOnlineSubsystem* const OnlineSubsystem = IOnlineSubsystem::Get();

	if (OnlineSubsystem)
	{
		IOnlineSessionPtr Sessions = OnlineSubsystem->GetSessionInterface();

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

			SessionSettings->Set(SETTING_MAPNAME, FString("L_Lobby"),
			                     EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);

			// Set the delegate to the handle of the session created function
			CreateSessionDelegateHandle = Sessions->AddOnCreateSessionCompleteDelegate_Handle(
				OnCreateSessionCompleteDelegate);

			// Create the session
			return Sessions->CreateSession(*UserId, SessionName, *SessionSettings);
		}
	}
	else
	{
		Print("No Online Subsystem found!");
	}
	return false;
}

void UBetrayalGameInstance::OnCreateSessionComplete(FName SessionName, bool bWasSuccessful)
{
	//GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("Session created: %s, %d"), *SessionName.ToString(), bWasSuccessful));
	Print("Session created: " + SessionName.ToString() + ", " + FString::FromInt(bWasSuccessful));
	
	IOnlineSubsystem* const OnlineSubsystem = IOnlineSubsystem::Get();
	if(!OnlineSubsystem)
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("No Online Subsystem found!")));
		return;
	}

	IOnlineSessionPtr Sessions = OnlineSubsystem->GetSessionInterface();
	if(!Sessions.IsValid())
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
	}
}

void UBetrayalGameInstance::OnStartOnlineGameComplete(FName SessionName, bool bWasSuccessful)
{
	Print("Session started: " + SessionName.ToString() + ", " + FString::FromInt(bWasSuccessful));

	IOnlineSubsystem* const OnlineSubsystem = IOnlineSubsystem::Get();
	if(!OnlineSubsystem)
	{
		Print("No Online Subsystem found!");
		return;
	}

	IOnlineSessionPtr Sessions = OnlineSubsystem->GetSessionInterface();
	if(!Sessions.IsValid())
	{
		Print("No Session Interface found!");
		return;
	}

	Sessions->ClearOnStartSessionCompleteDelegate_Handle(StartSessionDelegateHandle);

	if (bWasSuccessful)
	{
		// Travel to the lobby
		UGameplayStatics::OpenLevel(GetWorld(), "L_Lobby", true, "listen");
	}
}

void UBetrayalGameInstance::FindSessions(TSharedPtr<const FUniqueNetId> UserId, bool bIsLAN, bool bIsPresence)
{
	// Get the Online Subsystem
	IOnlineSubsystem* const OnlineSubsystem = IOnlineSubsystem::Get();
	if(!OnlineSubsystem)
	{
		Print("No Online Subsystem found!");
		return;
	}

	IOnlineSessionPtr Sessions = OnlineSubsystem->GetSessionInterface();

	if(Sessions.IsValid() && UserId.IsValid())
	{
		SessionSearch = MakeShareable(new FOnlineSessionSearch());

		SessionSearch->bIsLanQuery = bIsLAN;
		SessionSearch->MaxSearchResults = 10000;
		SessionSearch->PingBucketSize = 50;

		if (bIsPresence)
		{
			SessionSearch->QuerySettings.Set(SEARCH_PRESENCE, bIsPresence, EOnlineComparisonOp::Equals);
		}

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
	Print("Session search completed: " + FString::FromInt(bWasSuccessful));

	IOnlineSubsystem* const OnlineSubsystem = IOnlineSubsystem::Get();
	if(!OnlineSubsystem)
	{
		Print("No Online Subsystem found!");
		return;
	}

	IOnlineSessionPtr Sessions = OnlineSubsystem->GetSessionInterface();
	if(!Sessions.IsValid())
	{
		Print("No Session Interface found!");
		return;
	}

	Sessions->ClearOnFindSessionsCompleteDelegate_Handle(OnFindSessionsCompleteDelegateHandle);

	// Log number of results
	Print("Found results: " + FString::FromInt(SessionSearch->SearchResults.Num()));

	// Log info about each session
	for (int32 i = 0; i < SessionSearch->SearchResults.Num(); i++)
	{
		Print("Session " + FString::FromInt(i) + ": " + SessionSearch->SearchResults[i].Session.OwningUserName);
	}
}

bool UBetrayalGameInstance::JoinSession(TSharedPtr<const FUniqueNetId> UserId, FName SessionName,
	const FOnlineSessionSearchResult& SearchResult)
{
	bool bSuccessful = false;

	IOnlineSubsystem* const OnlineSubsystem = IOnlineSubsystem::Get();
	if(!OnlineSubsystem)
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
	if(!OnlineSubsystem)
	{
		Print("No Online Subsystem found!");
		return;
	}

	IOnlineSessionPtr Sessions = OnlineSubsystem->GetSessionInterface();
	if(!Sessions.IsValid())
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
			PlayerController->ClientTravel(TravelURL, ETravelType::TRAVEL_Absolute);
		}
	}
}

void UBetrayalGameInstance::OnDestroySessionComplete(FName SessionName, bool bWasSuccessful)
{
	Print("OnDestroySessionComplete: " + SessionName.ToString() + ", " + FString::FromInt(bWasSuccessful));

	IOnlineSubsystem* const OnlineSubsystem = IOnlineSubsystem::Get();
	if(!OnlineSubsystem)
	{
		Print("No Online Subsystem found!");
		return;
	}

	IOnlineSessionPtr Sessions = OnlineSubsystem->GetSessionInterface();
	if(!Sessions.IsValid())
	{
		Print("No Session Interface found!");
		return;
	}

	Sessions->ClearOnDestroySessionCompleteDelegate_Handle(OnDestroySessionCompleteDelegateHandle);

	if (bWasSuccessful)
	{
		// Travel to the lobby
		UGameplayStatics::OpenLevel(GetWorld(), "L_MainMenu", true);
	}
}

void UBetrayalGameInstance::StartOnlineGame(bool bIsLAN, bool bIsPresence, int32 MaxNumPlayers)
{
	const TSharedPtr<const FUniqueNetId> netID = UGameplayStatics::GetGameInstance(GetWorld())->GetFirstGamePlayer()->GetPreferredUniqueNetId().GetUniqueNetId();
	HostSession(netID, NAME_GameSession, bIsLAN, bIsPresence, MaxNumPlayers);
}

void UBetrayalGameInstance::FindOnlineGames(bool bIsLAN, bool bIsPresence)
{
	const TSharedPtr<const FUniqueNetId> netID = UGameplayStatics::GetGameInstance(GetWorld())->GetFirstGamePlayer()->GetPreferredUniqueNetId().GetUniqueNetId();
	FindSessions(netID, bIsLAN, bIsPresence);
}

void UBetrayalGameInstance::JoinOnlineGame(int32 SessionIndex)
{
	// TODO: Set up a way to select a session to join
	
	const TSharedPtr<const FUniqueNetId> netID = UGameplayStatics::GetGameInstance(GetWorld())->GetFirstGamePlayer()->GetPreferredUniqueNetId().GetUniqueNetId();

	// Search result
	FOnlineSessionSearchResult SearchResult;

	if (SessionSearch->SearchResults.Num() > 0)
	{
		for (int32 i = 0; i < SessionSearch->SearchResults.Num(); i++)
		{
			if (SessionSearch->SearchResults[i].Session.OwningUserId != netID)
			{
				SearchResult = SessionSearch->SearchResults[i];

				JoinSession(netID, NAME_GameSession, SearchResult);
				break;
			}
		}
	}
}

void UBetrayalGameInstance::DestroySessionAndLeaveGame()
{
	IOnlineSubsystem* const OnlineSubsystem = IOnlineSubsystem::Get();
	if(!OnlineSubsystem)
	{
		Print("No Online Subsystem found!");
		return;
	}

	IOnlineSessionPtr Sessions = OnlineSubsystem->GetSessionInterface();
	if(!Sessions.IsValid())
	{
		Print("No Session Interface found!");
		return;
	}

	Sessions->AddOnDestroySessionCompleteDelegate_Handle(OnDestroySessionCompleteDelegate);
	Sessions->DestroySession(NAME_GameSession);
}

#pragma endregion
