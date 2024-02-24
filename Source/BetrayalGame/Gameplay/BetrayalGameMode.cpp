// Fill out your copyright notice in the Description page of Project Settings.


#include "../Gameplay/BetrayalGameMode.h"

#include "BetrayalGame/AI/Controllers/AIPlayerController.h"
#include "Kismet/GameplayStatics.h"


ABetrayalGameMode::ABetrayalGameMode()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ABetrayalGameMode::BeginPlay()
{
	Super::BeginPlay();
	BetrayalGameState = GetGameState<ABetrayalGameState>();
	SetMatchStage(Preparing);
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

void ABetrayalGameMode::EnableAIPlayerControllers()
{
	TArray<AActor*> AIPlayerControllers;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AAIPlayerController::StaticClass(), AIPlayerControllers);
	for (AActor* ControllerActor : AIPlayerControllers)
	{
		if (AAIPlayerController* AIPlayerController = Cast<AAIPlayerController>(ControllerActor))
			AIPlayerController->EnableAIPlayer();
	}
}

void ABetrayalGameMode::SetMatchStage(TEnumAsByte<EMatchStage> NewStage)
{
	MatchStage = NewStage;
	StageTimer = 0;
	
	if (MatchStage == Preparing)
	{
		MaxStageTimer = PrepareStage.TimeLength;
		SetStageUseTimer(PrepareStage.bUsesTimer);
		OnPreparingStageStart();
	}
	else if (MatchStage == Exploring)
	{
		EnableAIPlayerControllers();
		
		MaxStageTimer = ExploreStage.TimeLength;		
		SetStageUseTimer(ExploreStage.bUsesTimer);
		OnExploringStageStart();
		StartMatch();
	}
	else if (MatchStage == Haunting)
	{
		MaxStageTimer = HauntStage.TimeLength;		
		SetStageUseTimer(HauntStage.bUsesTimer);
		OnHauntingStageStart();
	}
	else if (MatchStage == Finishing)
	{
		MaxStageTimer = FinishStage.TimeLength;		
		SetStageUseTimer(FinishStage.bUsesTimer);
		OnFinishingStageStart();
		EndMatch();
	}

	if (BetrayalGameState)
		BetrayalGameState->SetMatchStage(MatchStage);
}
