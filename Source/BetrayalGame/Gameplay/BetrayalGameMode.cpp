// Fill out your copyright notice in the Description page of Project Settings.


#include "../Gameplay/BetrayalGameMode.h"


ABetrayalGameMode::ABetrayalGameMode()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ABetrayalGameMode::BeginPlay()
{
	Super::BeginPlay();
	BetrayalGameState = GetGameState<ABetrayalGameState>();
	SetMatchStage(Lobby);
}

void ABetrayalGameMode::SetNextStage()
{
	const int NextStage = MatchStage.GetIntValue() + 1;
	SetMatchStage(static_cast<TEnumAsByte<EMatchStage>>(NextStage));
}

void ABetrayalGameMode::RunMatchTimer(float DeltaSeconds)
{
	StageTimer += DeltaSeconds;
	if (BetrayalGameState)
		BetrayalGameState->SetCountdown((MaxStageTimer - StageTimer) + 1);
	
	if (StageTimer >= MaxStageTimer)
	{
		SetNextStage();
	}
}

void ABetrayalGameMode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (bStageUsesTimer)
		RunMatchTimer(DeltaSeconds);
}

void ABetrayalGameMode::StartMatch()
{
	Super::StartMatch();

	if (!BetrayalGameState)
		BetrayalGameState = GetGameState<ABetrayalGameState>();

	
}

void ABetrayalGameMode::EndMatch()
{
	Super::EndMatch();
}

void ABetrayalGameMode::SetStageUseTimer(const bool bUseTimer)
{
	bStageUsesTimer = bUseTimer;		
	if (bStageUsesTimer && BetrayalGameState)
		BetrayalGameState->SetCountdown((MaxStageTimer - StageTimer) + 1);
	else if (!bStageUsesTimer && BetrayalGameState)	
		BetrayalGameState->SetCountdown(-1);
}

void ABetrayalGameMode::SetMatchStage(TEnumAsByte<EMatchStage> NewStage)
{
	MatchStage = NewStage;
	StageTimer = 0;
	
	if (MatchStage == Lobby)
	{
		MaxStageTimer = LobbyStage.TimeLength;
		SetStageUseTimer(LobbyStage.bUsesTimer);
		BetrayalGameState->OnMatchStageChanged(MatchStage);
	}
	else if (MatchStage == Exploring)
	{
		MaxStageTimer = ExploreStage.TimeLength;		
		SetStageUseTimer(ExploreStage.bUsesTimer);
		BetrayalGameState->OnMatchStageChanged(MatchStage);
		StartMatch();
	}
	else if (MatchStage == Haunting)
	{
		MaxStageTimer = HauntStage.TimeLength;		
		SetStageUseTimer(HauntStage.bUsesTimer);
		BetrayalGameState->OnMatchStageChanged(MatchStage);
	}
	else if (MatchStage == Finishing)
	{
		MaxStageTimer = FinishStage.TimeLength;		
		SetStageUseTimer(FinishStage.bUsesTimer);
		BetrayalGameState->OnMatchStageChanged(MatchStage);
		EndMatch();
	}

	if (BetrayalGameState)
		BetrayalGameState->SetMatchStage(MatchStage);
}
