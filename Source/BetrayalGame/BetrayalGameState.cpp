// Fill out your copyright notice in the Description page of Project Settings.


#include "BetrayalGameState.h"

#include "GameFramework/GameSession.h"
#include "Gameplay/BetrayalGameMode.h"
#include "Gameplay/BetrayalPlayerState.h"
#include "Gameplay/ObjectivesComponent.h"
#include "Kismet/GameplayStatics.h"
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

void ABetrayalGameState::BeginPlay()
{
	Super::BeginPlay();

	if(HasAuthority())
		InitializeHaunt();
}

void ABetrayalGameState::InitializeHaunt()
{
	// Get haunt data from data table and choose a random one
	{
		if(!HauntData)
			return;
	
	const int32 NumRows = HauntData->GetRowNames().Num();
	
	int32 RandomIndex = FMath::RandRange(0, NumRows - 1);
	
	FName RandomRowName = HauntData->GetRowNames()[RandomIndex];
	
	FHaunt* Haunt = HauntData->FindRow<FHaunt>(RandomRowName, "");
	
	if(Haunt)
		CurrentHaunt = *Haunt;
	}

	// Set haunt stage timer
	{
		if(CurrentHaunt.Duration > 0)
		{
			ABetrayalGameMode* BetrayalGameMode = Cast<ABetrayalGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
			if(BetrayalGameMode)
			{
				BetrayalGameMode->HauntStage.bUsesTimer = true;
				BetrayalGameMode->HauntStage.TimeLength = CurrentHaunt.Duration;
			}
		}
			
			
	}
	
}

void ABetrayalGameState::StartHaunt()
{
	// Set random player as traitor
	{
		const int32 NumPlayers = PlayerArray.Num();

		const int32 RandomPlayerIndex = FMath::RandRange(0, NumPlayers - 1);

		ABetrayalPlayerState* BetrayalPlayerState = Cast<ABetrayalPlayerState>(PlayerArray[RandomPlayerIndex]);
		if(!BetrayalPlayerState)
			return;

		BetrayalPlayerState->SetIsTraitor(true);
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Player %s is the traitor"), *BetrayalPlayerState->GetActorLabel()));
		
		APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(BetrayalPlayerState->GetPawn());
		if(!PlayerCharacter)
			return;
		
		OnTraitorChosen(PlayerCharacter);
	}
	
	// Assign objectives to players based on if they are traitor or not
	{
		const FObjective* InnocentObjective = CurrentHaunt.InnocentObjective.DataTable->FindRow<FObjective>(CurrentHaunt.InnocentObjective.RowName, "");
		const FObjective* TraitorObjective = CurrentHaunt.TraitorObjective.DataTable->FindRow<FObjective>(CurrentHaunt.TraitorObjective.RowName, "");
		if(!InnocentObjective || !TraitorObjective)
			return;

		// Set objectives for players
		for (auto Player : PlayerArray)
		{
			const ABetrayalPlayerState* BPlayerState = Cast<ABetrayalPlayerState>(Player);
			if(!BPlayerState)
				continue;

			const APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(BPlayerState->GetPawn());
			if (!PlayerCharacter)
				continue;

			if (BPlayerState->IsTraitor())
				PlayerCharacter->ObjectivesComponent->Server_SetHauntObjective(*TraitorObjective);
			else
				PlayerCharacter->ObjectivesComponent->Server_SetHauntObjective(*InnocentObjective);
		}
	}
}

void ABetrayalGameState::OnInnocentWin_Implementation()
{
	
}

void ABetrayalGameState::OnTraitorWin_Implementation()
{
	
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



