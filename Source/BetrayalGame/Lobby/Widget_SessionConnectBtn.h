// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
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
	UPROPERTY(BlueprintReadWrite, Category = "Lobby")
	FName _SessionName;

	UPROPERTY(BlueprintReadWrite, Category = "Lobby")
	int32 _ConnectedPlayers;

	UPROPERTY(BlueprintReadWrite, Category = "Lobby")
	int32 _MaxPlayers;

	UPROPERTY(BlueprintReadWrite, Category = "Lobby")
	int32 _Ping;

	UPROPERTY(BlueprintReadWrite, Category = "Lobby")
	int32 _SearchResultsIndex;
	
};
