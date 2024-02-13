// Fill out your copyright notice in the Description page of Project Settings.


#include "BetrayalGameState.h"

#include "Net/UnrealNetwork.h"

ABetrayalGameState::ABetrayalGameState()
{
	bReplicates = true;
}

void ABetrayalGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABetrayalGameState, MatchStage);
	DOREPLIFETIME(ABetrayalGameState, Countdown);
}
