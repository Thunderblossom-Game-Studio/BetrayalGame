// Fill out your copyright notice in the Description page of Project Settings.


#include "BetrayalGameState.h"
#include "BetrayalPlayerState.h"
#include "Net/UnrealNetwork.h"
#include "Engine/DataTable.h"

ABetrayalGameState::ABetrayalGameState(): CurrentHaunt(nullptr)
{
	bReplicates = true;
}

void ABetrayalGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABetrayalGameState, MatchStage);
	DOREPLIFETIME(ABetrayalGameState, Countdown);
	DOREPLIFETIME(ABetrayalGameState, CurrentHaunt);
}

void ABetrayalGameState::BeginPlay()
{
	Super::BeginPlay();
	

}

void ABetrayalGameState::OnMatchStageChanged_Implementation(const EMatchStage NewStage)
{
	switch (NewStage)
	{
	case Lobby:
		OnLobbyStageStart();
		break;
	case Exploring:
		OnExploringStageStart();
		break;
	case Haunting:
		OnHauntingStageStart();
		CurrentHaunt->Server_StartHaunt();
		break;
	case Finishing:
		OnFinishingStageStart();
		CurrentHaunt->Server_EndHaunt();
		break;
	default:
		break;
	}
}



