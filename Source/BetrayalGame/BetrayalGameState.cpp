// Fill out your copyright notice in the Description page of Project Settings.


#include "BetrayalGameState.h"

#include "GameFramework/GameSession.h"
#include "Gameplay/BetrayalPlayerState.h"
#include "Gameplay/ObjectivesComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

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
	
	const int32 NumRows = HauntData->GetRowMap().Num();
	
	int32 RandomIndex = FMath::RandRange(0, NumRows - 1);
	
	FName RandomRowName = HauntData->GetRowNames()[RandomIndex];
	
	FHaunt* Haunt = HauntData->FindRow<FHaunt>(RandomRowName, "");
	
	if(Haunt)
		CurrentHaunt = *Haunt;

	// Set random player as traitor

	// Get random player from player array
	const int32 NumPlayers = PlayerArray.Num();

	const int32 RandomPlayerIndex = FMath::RandRange(0, NumPlayers - 1);

	ABetrayalPlayerState* BetrayalPlayerState = Cast<ABetrayalPlayerState>(PlayerArray[RandomPlayerIndex]);
	if(!BetrayalPlayerState)
		return;

	BetrayalPlayerState->SetIsTraitor(true);
	
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Player %s is the traitor"), *BetrayalPlayerState->GetActorLabel()));
}

void ABetrayalGameState::AddObjectivesToPlayers_Implementation()
{
	const FObjective* InnocentObjective = CurrentHaunt.InnocentObjective.DataTable->FindRow<FObjective>(CurrentHaunt.InnocentObjective.RowName, "");
	const FObjective* TraitorObjective = CurrentHaunt.TraitorObjective.DataTable->FindRow<FObjective>(CurrentHaunt.TraitorObjective.RowName, "");
	if(!InnocentObjective || !TraitorObjective)
		return;

	APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(),0));
	if (!PlayerCharacter)
		return;

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "Adding objectives to players");

	// const FObjective* TraitorObjective = CurrentHaunt.TraitorObjective.DataTable->FindRow<FObjective>(CurrentHaunt.TraitorObjective.RowName, "");
	//
	//
	//
	// for (auto Player : PlayerArray)
	// {
	// 	
	//
	// 	APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(BetrayalPlayerState->GetPawn());
	// 	if (!PlayerCharacter)
	// 		continue;
	//
	// 	if (BetrayalPlayerState->IsTraitor())
	// 	{
	// 		PlayerCharacter->ObjectivesComponent->SetHauntObjective(*TraitorObjective);
	// 	}
	// 	else
	// 	{
	// 		PlayerCharacter->ObjectivesComponent->SetEventObjective(*InnocentObjective);
	// 	}
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
		if(HasAuthority())
			StartHaunt();
		break;
	case Finishing:
		OnFinishingStageStart();
		break;
	default:
		break;
	}
}



