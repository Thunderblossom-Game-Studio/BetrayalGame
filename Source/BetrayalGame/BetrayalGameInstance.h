// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "GameFramework/SaveGame.h"
#include "OnlineSessionSettings.h"
#include "Interfaces/OnlineSessionDelegates.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "BetrayalGameInstance.generated.h"

#pragma region Save/Load
USTRUCT(BlueprintType)
struct FPlayerProfile
{
	GENERATED_BODY()
	FString PlayerName;
};
#pragma endregion

/**
 * 
 */
UCLASS()
class BETRAYALGAME_API UBetrayalGameInstance : public UGameInstance
{
	GENERATED_BODY()

#pragma region General

	UBetrayalGameInstance(const FObjectInitializer& ObjectInitializer);
	
	UFUNCTION(BlueprintCallable, Category = "General")
	void QuitGame();
	
#pragma endregion
	
#pragma region UI
private:
	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<UUserWidget> WB_MainMenuClass;

	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<UUserWidget> WB_LobbyClass;
	
public:
	UPROPERTY(BlueprintReadOnly, Category = "UI")
	UUserWidget* WB_MainMenu;
	
	UPROPERTY(BlueprintReadOnly, Category = "UI")
	UUserWidget* WB_Lobby;

	UFUNCTION(BlueprintCallable, Category = "UI")
	void ShowMainMenu();

	UFUNCTION(BlueprintCallable, Category = "UI")
	void HideMainMenu();

	UFUNCTION(BlueprintCallable, Category = "UI")
	void ShowLobby();

	UFUNCTION(BlueprintCallable, Category = "UI")
	void HideLobby();
#pragma endregion

#pragma region Save/Load
private:
	UPROPERTY()
	USaveGame* SG_PlayerProfile;

	UPROPERTY()
	FString SaveSlotName = "UserProfile";

	UPROPERTY()
	FPlayerProfile PlayerProfileData;

public:
	UFUNCTION(BlueprintCallable, Category = "Save/Load")
	void SavePlayerProfile();

	UFUNCTION(BlueprintCallable, Category = "Save/Load")
	void LoadPlayerProfile();

	UFUNCTION(BlueprintCallable, Category = "Save/Load")
	void CheckPlayerProfile();
	
#pragma endregion

#pragma region Networking

private:
	const TSharedPtr<const FUniqueNetId> GetNetID();

	UPROPERTY(EditAnywhere, Category = "Networking")
	FString LevelToLoad = "";
	
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

	// Internal create session method
	UFUNCTION()
	void UI_HostGame();

	UFUNCTION()
	void UI_JoinGame();
	
	/**
	*	Create a joinable session
	*
	*	@Param		UserID			UserID of Session Owner
	*	@Param		SessionName		Name of the Session
	*	@Param		bIsLAN			Is LAN connection or Online connection
	*	@Param		bIsPresence		Create a presence session (i.e: Discord rich presence)
	*	@Param		MaxNumPlayers	Total number of allowed connections
	*/
	bool HostSession(TSharedPtr<const FUniqueNetId> UserId, FName SessionName, bool bIsLAN, bool bIsPresence, int32 MaxNumPlayers);

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

public:
	// Clear sessions
	UFUNCTION(BlueprintImplementableEvent, Category = "Networking")
	void ClearSessions();

	// Add session to UI
	UFUNCTION(BlueprintImplementableEvent, Category = "Networking")
	void AddSessionToList(FName SessionName, int32 ConnectedPlayers, int32 MaxPlayers, int32 Ping, int32 SearchResultsIndex);
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

public:
#pragma region Blueprint Nodes
	UFUNCTION(BlueprintCallable, Category = "Networking")
	/**	Host a game
	 * 
	 * @param bIsLAN		Is LAN connection or Online connection
	 * @param bIsPresence   Create a presence session (i.e: Discord rich presence)
	 * @param MaxNumPlayers Total number of allowed connections 
	 */
	void StartOnlineGame(bool bIsLAN, bool bIsPresence, int32 MaxNumPlayers);

	UFUNCTION(BlueprintCallable, Category = "Networking")
	/**	Find an online session
	 * 
	 * @param bIsLAN		Are we searching LAN matches
	 * @param bIsPresence	Are we searching presence sessions
	 */
	void FindOnlineGames(bool bIsLAN, bool bIsPresence);

	UFUNCTION(BlueprintCallable, Category = "Networking")
	/**	Join an online session
	 * 
	 * @param SessionIndex	Index of the session to join
	 */
	void JoinOnlineGame(int32 SessionIndex);

	UFUNCTION(BlueprintCallable, Category = "Networking")
	/**	Destroy an online session
	 * 
	 */
	void DestroySessionAndLeaveGame();
#pragma endregion
#pragma endregion
	
};
