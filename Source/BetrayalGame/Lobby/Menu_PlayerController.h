// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Menu_PlayerController.generated.h"

/**
 * 
 */
UCLASS()
class BETRAYALGAME_API AMenu_PlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AMenu_PlayerController();

protected:
	virtual void BeginPlay() override;

	void Login();
	void OnLoginCompleted(int32 LocalUserNum, bool bWasSuccessful, const FUniqueNetId& UserId, const FString& Error);

	FDelegateHandle LoginDelegateHandle;
};
