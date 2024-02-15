// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "Subsystems/WorldSubsystem.h"
#include "BetrayalGameNetworkSubsystem.generated.h"

// Forward Declarations
class UBetrayalGameInstance;

// Type Definitions
const FName PASSWORD = "PASSWORD";

UCLASS()
class BETRAYALGAME_API UBetrayalGameNetworkSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

private:
	UPROPERTY()
	UBetrayalGameInstance* _GameInstance;
	
	const TSharedPtr<const FUniqueNetId> GetNetID();
	
public:
	// Initialize
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	UPROPERTY(BlueprintReadOnly, Category = "Networking")
	int MAX_PLAYERS = 4; // Max number of players in a session

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Networking")
	FName LevelToLoad = "";

	// Found sessions
	UPROPERTY(BlueprintReadOnly, Category = "Networking")
	TArray<UUserWidget*> FoundSessionButtons;

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
	
	/**
	*	Create a joinable session
	*
	*	@Param		UserID			UserID of Session Owner
	*	@Param		SessionName		Name of the Session
	*	@Param		bIsLAN			Is LAN connection or Online connection
	*	@Param		bIsPresence		Create a presence session (i.e: Discord rich presence)
	*	@Param		MaxNumPlayers	Total number of allowed connections
	*/
	bool HostSession(TSharedPtr<const FUniqueNetId> UserId, FName SessionName, bool bIsLAN, bool bIsPresence, int32 MaxNumPlayers, bool bIsPrivate, FString Password);

	// Blueprint callable function to start a session
	UFUNCTION(BlueprintCallable, Category = "Networking")
	void BP_HostSession(FName SessionName, bool bIsLAN, bool bIsPresence, int32 MaxNumPlayers, bool bIsPrivate, FString Password);

	/**
	*	Delegate function fired on creating session
	*
	*	@Param		SessionName		Name of the session this callback is for
	*	@Param		bWasSuccessful	True if async action completed without error
	*/
	virtual void OnCreateSessionComplete(FName SessionName, bool bWasSuccessful);

	/**
	*	Delegate function fired on starting session
	*
	*	@Param		SessionName		Name of the session this callback is for
	*	@Param		bWasSuccessful	True if async action completed without error
	*/
	void OnStartOnlineGameComplete(FName SessionName, bool bWasSuccessful);
#pragma endregion

#pragma region Session Finding
	/**
	*	find an Online Session
	*
	*	@Param		UserId			User that initiated the request
	*	@Param		bIsLan			Are we searching LAN matches
	*	@Param		bIsPresence		Are we searching presence sessions
	*/
	void FindSessions(TSharedPtr<const FUniqueNetId> UserId, bool bIsLAN, bool bIsPresence);

	// Blueprint callable function to find sessions
	UFUNCTION(BlueprintCallable, Category = "Networking")
	void BP_FindSessions(bool bIsLAN, bool bIsPresence);

	// Delegate for searching for sessions
	FOnFindSessionsCompleteDelegate OnFindSessionsCompleteDelegate;

	// Handle to registered delegate for searching a session
	FDelegateHandle OnFindSessionsCompleteDelegateHandle;

	// Session Search
	TSharedPtr<class FOnlineSessionSearch> SessionSearch;

	/**
	 * Delegate fired when a session search query has completed
	 *
	 * @param bWasSuccessful true if the async action completed without error, false if there was an error
	 */
	void OnFindSessionsComplete(bool bWasSuccessful);
#pragma endregion

#pragma region Session Joining
	/**
	*	Join a session via a search result
	*
	*	@Param		SessionName		Name of the session
	*	@Param		SearchResult	Session to join
	*
	*	@return		bool			True if successful, false otherwise
	*/
	bool JoinSession(TSharedPtr<const FUniqueNetId> UserId, FName SessionName, const FOnlineSessionSearchResult& SearchResult);

	
	bool JoinSession(FName SessionName, const FOnlineSessionSearchResult& SearchResult);

	// Delegate for joining a session
	FOnJoinSessionCompleteDelegate OnJoinSessionCompleteDelegate;

	// Handle to registered delegate for joining a session
	FDelegateHandle OnJoinSessionCompleteDelegateHandle;

	/**
	 *	Delegate fired when a session join request has completed
	 *
	 *	@param	SessionName		Name of session this callback is for
	 *	@param	Result			Result of the async action
	 */
	void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);

	// Invite accepted delegate
	FDelegateHandle OnSessionUserInviteAcceptedDelegateHandle;
	FOnSessionUserInviteAcceptedDelegate OnSessionUserInviteAcceptedDelegate;

	void OnSessionUserInviteAccepted(const bool bWasSuccesful, const int32 ControllerId, TSharedPtr<const FUniqueNetId> UserId, const FOnlineSessionSearchResult& InviteResult);

public:
	// Clear sessions
	//UFUNCTION(BlueprintImplementableEvent, Category = "Networking")
	//void ClearSessions();

	// Add session to UI
	//UFUNCTION(BlueprintImplementableEvent, Category = "Networking")
	//void AddSessionToList(FSessionData SessionData);
private:
#pragma endregion

#pragma region Session Destruction
	// Delegate for destroying a session
	FOnDestroySessionCompleteDelegate OnDestroySessionCompleteDelegate;

	// Handle to registered delegate for destroying a session
	FDelegateHandle OnDestroySessionCompleteDelegateHandle;

	/**
	 *	Delegate fired when a destroying an online session has completed
	 *
	 *	@param	SessionName		Name of session this callback is for
	 *	@param	bWasSuccessful	true if the async action completed without error, false if there was an error
	 */
	void OnDestroySessionComplete(FName SessionName, bool bWasSuccessful); 
#pragma endregion
	
};