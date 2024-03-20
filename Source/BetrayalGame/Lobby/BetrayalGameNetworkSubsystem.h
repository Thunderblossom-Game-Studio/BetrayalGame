// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameDelegates.h"
#include "Engine/SoftWorldReference.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "Subsystems/WorldSubsystem.h"
#include "Net/Core/Connection/NetEnums.h"
#include "BetrayalGame/Lobby/Widget_PlayerNameText.h"
#include "BetrayalGameNetworkSubsystem.generated.h"

// Forward Declarations
class UBetrayalGameInstance;

// Type Definitions
const FName PASSWORD = "PASSWORD";
const FName SERVERLIST_NAME = "SESSION_NAME";

UENUM(BlueprintType)
enum class ESessionSearchResult : uint8
{
	SSR_Success UMETA(DisplayName = "Success"),
	SSR_Failure UMETA(DisplayName = "Failure")
};

UENUM(BlueprintType)
enum class ESessionPrivacy : uint8
{
	SP_Public UMETA(DisplayName = "Public"),
	SP_Friends UMETA(DisplayName = "Friends"),
	SP_Private UMETA(DisplayName = "Private"),
	SP_LAN UMETA(DisplayName = "LAN")
};

UCLASS()
class BETRAYALGAME_API UBetrayalGameNetworkSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()


	// Delegates for client connection state changing callbacks
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnClientConnectedDelegate);

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnClientDisconnectedDelegate);

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnClientsChangedDelegate);

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	UPROPERTY()
	UBetrayalGameInstance* _GameInstance;

	UPROPERTY()
	class AMenu_PlayerController* _MenuController;

	// Helper function to grab the net ID
	const TSharedPtr<const FUniqueNetId> GetNetID();

	// Helper function to grab session interface
	IOnlineSessionPtr GetSessionInterface();

	// Set up network notifications (client connected, client disconnected, etc.)
	void SetupNotifications();
	void CleanupNotifications();

	// State change delegates
	FOnClientConnectedDelegate OnClientConnectedDelegate;
	FOnClientDisconnectedDelegate OnClientDisconnectedDelegate;
	FOnClientsChangedDelegate OnClientsChangedDelegate;

	// State change functions
	void OnClientConnected(FName SessionName, const FUniqueNetId& ID);
	void OnClientDisconnected(FName SessionName, const FUniqueNetId& ID, EOnSessionParticipantLeftReason Reason);

	void HandleNetworkFailure(UWorld* World, UNetDriver* NetDriver, ENetworkFailure::Type FailureType,
	                          const FString& ErrorString);

	// TODO: Change this to correct value for release, currently higher for testing
	UPROPERTY(BlueprintReadOnly, Category = "Networking")
	int MAX_PLAYERS = 8; // Max number of players in a session

	// Name displayed in server browser
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Networking")
	FString LobbyListName = "BetrayalGameSession";

	// Found sessions
	UPROPERTY(BlueprintReadOnly, Category = "Networking")
	TArray<UUserWidget*> FoundSessionButtons;

	// Reset session search
	UFUNCTION(BlueprintCallable, Category = "Networking")
	void ResetSessionSearch();

	// Function to lock session, stopping mid-game connections or invites
	UFUNCTION(BlueprintCallable, Category = "Networking")
	void LockSession();

	// Function to change map by name reference
	UFUNCTION(BlueprintCallable, Category = "Networking")
	void ChangeMapByName(FName MapName);

	// Function to change map by object reference
	UFUNCTION(BlueprintCallable, Category = "Networking")
	void ChangeMapByReference(FSoftWorldReference Map);

	UFUNCTION(BlueprintImplementableEvent, Category = "Networking")
	void UpdatePlayerNameText(UWidget_PlayerNameText* PlayerNameText);

#pragma region Session Creation
	// Session created delegate
	FOnCreateSessionCompleteDelegate OnCreateSessionCompleteDelegate;

	// Session start delegate
	FOnStartSessionCompleteDelegate OnStartSessionCompleteDelegate;

	// Delegate Handles
	FDelegateHandle CreateSessionDelegateHandle;
	FDelegateHandle StartSessionDelegateHandle;

	// Online session settings
	TSharedPtr<class FOnlineSessionSettings> SessionSettings;

	bool HostSession(TSharedPtr<const FUniqueNetId> UserId, FName SessionName, FString Password, ESessionPrivacy Privacy);

	// Blueprint callable function to start a session
	UFUNCTION(BlueprintCallable, Category = "Networking")
	void BP_HostSession(FName SessionName, FString Password, ESessionPrivacy Privacy);

	// Post session complete delegate callback
	virtual void OnCreateSessionComplete(FName SessionName, bool bWasSuccessful);

	// Post start session complete delegate callback
	void OnStartOnlineGameComplete(FName SessionName, bool bWasSuccessful);
#pragma endregion

#pragma region Session Finding
	// Function to find sessions
	ESessionSearchResult FindSessions(TSharedPtr<const FUniqueNetId> UserId, bool bIsLAN, bool bIsPresence);

	// Blueprint callable function to find sessions
	UFUNCTION(BlueprintCallable, Category = "Networking", Meta = (ExpandEnumAsExecs="Result"))
	void BP_FindSessions(bool bIsLAN, bool bIsPresence, ESessionSearchResult& Result);

	// Delegate for searching for sessions
	FOnFindSessionsCompleteDelegate OnFindSessionsCompleteDelegate;

	// Handle to registered delegate for searching a session
	FDelegateHandle OnFindSessionsCompleteDelegateHandle;

	// Session Search
	TSharedPtr<class FOnlineSessionSearch> SessionSearch;

	// Post session search complete delegate callback
	void OnFindSessionsComplete(bool bWasSuccessful);
#pragma endregion

#pragma region Session Joining

	// Join session functions with overloads 
	bool JoinSession(TSharedPtr<const FUniqueNetId> UserId, FName SessionName,
	                 const FOnlineSessionSearchResult& SearchResult);
	bool JoinSession(FName SessionName, const FOnlineSessionSearchResult& SearchResult);

	// Delegate for joining a session
	FOnJoinSessionCompleteDelegate OnJoinSessionCompleteDelegate;

	// Handle to registered delegate for joining a session
	FDelegateHandle OnJoinSessionCompleteDelegateHandle;

	// Post session join complete delegate callback
	void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);

	// Invite accepted delegate
	FDelegateHandle OnSessionUserInviteAcceptedDelegateHandle;
	FOnSessionUserInviteAcceptedDelegate OnSessionUserInviteAcceptedDelegate;

	// Post invite accepted delegate callback
	void OnSessionUserInviteAccepted(const bool bWasSuccesful, const int32 ControllerId,
	                                 TSharedPtr<const FUniqueNetId> UserId,
	                                 const FOnlineSessionSearchResult& InviteResult);
#pragma endregion

#pragma region Session Destruction

private:
	// Delegate for destroying a session
	FOnDestroySessionCompleteDelegate OnDestroySessionCompleteDelegate;

	// Handle to registered delegate for destroying a session
	FDelegateHandle OnDestroySessionCompleteDelegateHandle;

	// Post session destroy complete delegate callback
	void OnDestroySessionComplete(FName SessionName, bool bWasSuccessful);

	// Blueprint callable function to destroy a session
	UFUNCTION(BlueprintCallable, Category = "Networking")
	void BP_DestroySession();

	// Delegate to handle a disconnect from a session
	FHandleDisconnectDelegate OnHandleDisconnectDelegate;

	// Handle to registered delegate for handling a disconnect from a session
	FDelegateHandle OnHandleDisconnectDelegateHandle;
#pragma endregion
};
