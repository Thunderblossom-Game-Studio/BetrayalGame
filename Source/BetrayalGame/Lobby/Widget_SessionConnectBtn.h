// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BetrayalGame/BetrayalGameInstance.h"
#include "Blueprint/UserWidget.h"
#include "Widget_SessionConnectBtn.generated.h"

/**
 * 
 */
UCLASS()
class BETRAYALGAME_API UWidget_SessionConnectBtn : public UUserWidget
{
	GENERATED_BODY()
	
public:
	// Search Result
	FOnlineSessionSearchResult _SessionData;
	
	// Displayed data
	UPROPERTY(BlueprintReadWrite, Category = "Lobby")
	FName _SessionName;

	UPROPERTY(BlueprintReadWrite, Category = "Lobby")
	int32 _ConnectedPlayers;

	UPROPERTY(BlueprintReadWrite, Category = "Lobby")
	int32 _MaxPlayers;

	UPROPERTY(BlueprintReadWrite, Category = "Lobby")
	int32 _Ping;
	// End of displayed data

	// Index in the search results array
	UPROPERTY(BlueprintReadWrite, Category = "Lobby")
	int32 _SearchResultsIndex;

	// If the session is private
	UPROPERTY(BlueprintReadOnly, Category = "Lobby")
	bool _bPrivate;

	UFUNCTION(BlueprintCallable, Category = "Lobby")
	bool CheckPassword();

	// Construct event
	virtual void NativeConstruct() override;

	// Join the session
	UFUNCTION()
	void OnClick();
	
};
