// Fill out your copyright notice in the Description page of Project Settings.


#include "BetrayalGameState.h"

#include "GameFramework/GameSession.h"
#include "Gameplay/ObjectivesComponent.h"
#include "Net/UnrealNetwork.h"
#include "Engine/DataTable.h"

ABetrayalGameState::ABetrayalGameState()
{
	bReplicates = true;

	HauntEvent.AddUObject(this, &ABetrayalGameState::StartHaunt);
}

void ABetrayalGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABetrayalGameState, MatchStage);
	DOREPLIFETIME(ABetrayalGameState, Countdown);
	DOREPLIFETIME(ABetrayalGameState, CurrentHaunt);
}

void ABetrayalGameState::StartHaunt()
{
	if(!HauntData)
		return;
	
	const int32 NumRows = HauntData->GetRowNames().Num();

	//GLog->Log(FString::FromInt(NumRows));
	
	const int32 RandomIndex = FMath::RandRange(1, NumRows - 1);

	const FName RandomRowName = HauntData->GetRowNames()[RandomIndex];

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, RandomRowName.ToString());
	
	//
	// FHaunt* Haunt = HauntData->FindRow<FHaunt>(RandomRowName, "");
	//
	// if(Haunt)
	// 	CurrentHaunt = *Haunt;
	//
	// FObjective InnocentObjective = *CurrentHaunt.InnocentObjective.DataTable->FindRow<FObjective>(CurrentHaunt.InnocentObjective.RowName, "");
	// FObjective TraitorObjective = *CurrentHaunt.TraitorObjective.DataTable->FindRow<FObjective>(CurrentHaunt.TraitorObjective.RowName, "");
	//
	// for (auto Player : PlayerArray)
	// {
	// 	APlayerCharacter* Character = Cast<APlayerCharacter>(Player);
	// 	if(Character)
	// 		Character->ObjectivesComponent->SetHauntObjective(InnocentObjective);
	// 	
	// }
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
		HauntEvent.Broadcast();
		break;
	case Finishing:
		OnFinishingStageStart();
		break;
	default:
		break;
	}
}



