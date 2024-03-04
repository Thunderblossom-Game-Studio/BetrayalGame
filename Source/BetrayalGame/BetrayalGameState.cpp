// Fill out your copyright notice in the Description page of Project Settings.


#include "BetrayalGameState.h"

#include "K2Node_SpawnActorFromClass.h"
#include "GameFramework/GameSession.h"
#include "Gameplay/BetrayalGameMode.h"
#include "Gameplay/BetrayalPlayerState.h"
#include "Gameplay/ObjectivesComponent.h"
#include "Kismet/GameplayStatics.h"
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

	CurrentHaunt = HauntClass.GetDefaultObject();
	CurrentHaunt->SetGameState(this);

	
}

ABetrayalPlayerState* ABetrayalGameState::GetRandomPlayer() const
{
	const int32 NumPlayers = PlayerArray.Num();

	const int32 RandomPlayerIndex = FMath::RandRange(0, NumPlayers - 1);

	return Cast<ABetrayalPlayerState>(PlayerArray[RandomPlayerIndex]);
}

TArray<ABetrayalPlayerState*> ABetrayalGameState::GetAllPlayers() const
{
	TArray<ABetrayalPlayerState*> OutPlayers;
	for (auto Player : PlayerArray)
	{
		OutPlayers.Add(Cast<ABetrayalPlayerState>(Player));
	}
	return OutPlayers;
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
		if(HasAuthority())
			CurrentHaunt->StartHaunt();
		break;
	case Finishing:
		OnFinishingStageStart();
		break;
	default:
		break;
	}
}



