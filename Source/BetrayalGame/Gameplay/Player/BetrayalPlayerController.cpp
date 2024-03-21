// Fill out your copyright notice in the Description page of Project Settings.


#include "BetrayalPlayerController.h"

#include "BetrayalGame/BetrayalPlayerState.h"

void ABetrayalPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if(HasAuthority())
	{
		ABetrayalPlayerState* BetrayalPlayerState = GetPlayerState<ABetrayalPlayerState>();
		if(IsValid(BetrayalPlayerState))
		{
			BetrayalPlayerState->SetPlayerName("Dave");
		}
		
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, BetrayalPlayerState->GetPlayerName());
		
	}
	
	
}
