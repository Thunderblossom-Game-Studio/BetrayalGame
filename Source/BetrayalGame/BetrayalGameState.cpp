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

void ABetrayalGameState::StartHaunt()
{
	const int32 NumRows = HauntTable.DataTable->GetTableData().Num();
	
	const int32 RandomIndex = FMath::RandRange(0, NumRows - 1);

	const FName RandomRowName = HauntTable.DataTable->GetRowNames()[RandomIndex];

	FHaunt* Haunt = HauntTable.DataTable->FindRow<FHaunt>(RandomRowName, "");

	if(Haunt)
		CurrentHaunt = *Haunt;
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
		break;
	case Finishing:
		OnFinishingStageStart();
		break;
	default:
		break;
	}
}



