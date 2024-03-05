// Fill out your copyright notice in the Description page of Project Settings.


#include "BetrayalGameState.h"

#include "K2Node_SpawnActorFromClass.h"
#include "GameFramework/GameSession.h"
#include "BetrayalGameMode.h"
#include "BetrayalPlayerState.h"
#include "Gameplay/Player/Player Components/ObjectivesComponent.h"
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
	

}

ABetrayalPlayerState* ABetrayalGameState::GetRandomPlayer() const
{
	const int32 NumPlayers = PlayerArray.Num();

	const int32 RandomPlayerIndex = FMath::RandRange(0, NumPlayers - 1);

	return Cast<ABetrayalPlayerState>(PlayerArray[RandomPlayerIndex]);
}

void ABetrayalGameState::HauntSetup()
{
	if(HasAuthority())
	{
		CurrentHaunt = HauntClass.GetDefaultObject();
		CurrentHaunt->SetGameState(this);
		CurrentHaunt = GetWorld()->SpawnActor<ABaseHaunt>(HauntClass);

		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Emerald, TEXT("Haunt Spawned"));
	}
}

void ABetrayalGameState::OnMatchStageChanged_Implementation(const EMatchStage NewStage)
{
	switch (NewStage)
	{
	case Lobby:
		OnLobbyStageStart();
		HauntSetup();
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



