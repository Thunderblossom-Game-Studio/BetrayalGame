// Fill out your copyright notice in the Description page of Project Settings.


#include "BetrayalGameMode.h"
#include "BetrayalGame/AI/Controllers/AIPlayerController.h"
#include "Gameplay/Haunts/HiddenAsymmetricalHaunt.h"
#include "Kismet/GameplayStatics.h"


ABetrayalGameMode::ABetrayalGameMode()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ABetrayalGameMode::BeginPlay()
{
	Super::BeginPlay();
	BetrayalGameState = GetGameState<ABetrayalGameState>();
	SetMatchStage(Lobby);

	SetupHaunt();	
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

void ABetrayalGameMode::EnableAIPlayerHauntMode()
{
	TArray<AActor*> AIPlayerControllers;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AAIPlayerController::StaticClass(), AIPlayerControllers);
	for (AActor* ControllerActor : AIPlayerControllers)
	{
		if (AAIPlayerController* AIPlayerController = Cast<AAIPlayerController>(ControllerActor))
			AIPlayerController->SetHauntBehaviours();
	}
}

TArray<ABetrayalPlayerState*> ABetrayalGameMode::GetAllPlayerStates() const
{
	TArray<ABetrayalPlayerState*> OutPlayers;
	for (auto Player : GameState->PlayerArray)
	{
		OutPlayers.Add(Cast<ABetrayalPlayerState>(Player));
	}
	return OutPlayers;
}

TArray<APlayerCharacter*> ABetrayalGameMode::GetAllPlayerCharacters() const
{
	TArray<APlayerCharacter*> OutCharacters;
	for (auto Player : GameState->PlayerArray)
	{
		OutCharacters.Add(Cast<APlayerCharacter>(Player));
	}
	
	return OutCharacters;
}

ABetrayalPlayerState* ABetrayalGameMode::GetRandomPlayer() const
{
	TArray<ABetrayalPlayerState*> AllPlayers = GetAllPlayerStates();
	if (AllPlayers.Num() > 0)
	{
		const int RandomIndex = FMath::RandRange(0, AllPlayers.Num() - 1);
		return AllPlayers[RandomIndex];
	}
	return nullptr;
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
		EnableAIPlayerControllers();
		
		MaxStageTimer = ExploreStage.TimeLength;		
		SetStageUseTimer(ExploreStage.bUsesTimer);
		BetrayalGameState->OnMatchStageChanged(MatchStage);
		StartMatch();
	}
	else if (MatchStage == Haunting)
	{
		EnableAIPlayerHauntMode();
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

void ABetrayalGameMode::SetupHaunt()
{
	if(!HauntClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("Haunt Class Not Set. Please set Haunt Class in GameMode Blueprint."));
		return;
	}
	BetrayalGameState->SetCurrentHaunt(HauntClass.GetDefaultObject());

	// Spawn CurrentHaunt in game world
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	BetrayalGameState->CurrentHaunt = GetWorld()->SpawnActor<ABaseHaunt>(HauntClass, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Emerald, TEXT("Haunt Actor Spawned. Owner: " + BetrayalGameState->CurrentHaunt->GetOwner()->GetName()));

	HauntStage.bUsesTimer = BetrayalGameState->CurrentHaunt->HasTimer();
	HauntStage.TimeLength = BetrayalGameState->CurrentHaunt->GetHauntDuration();
}
