// Fill out your copyright notice in the Description page of Project Settings.


#include "BetrayalGameMode.h"

#include "BetrayalGameInstance.h"
#include "BetrayalGame/AI/Controllers/AIPlayerController.h"
#include "GameFramework/GameSession.h"
#include "GameFramework/PlayerStart.h"
#include "Gameplay/Haunts/HiddenAsymmetricalHaunt.h"
#include "Gameplay/Player/BetrayalPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Lobby/BetrayalGameNetworkSubsystem.h"


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

void ABetrayalGameMode::EndPlay(const EEndPlayReason::Type EndPlayReason)
{		
	Super::EndPlay(EndPlayReason);	
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

void ABetrayalGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	
	//if (bUsesBots)
	//	ReplaceBot(Cast<ABetrayalPlayerController>(NewPlayer));
}

void ABetrayalGameMode::Logout(AController* Exiting)
{
	//ReplacePlayer(Cast<ABetrayalPlayerController>(Exiting));
	
	Super::Logout(Exiting);
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
	if (!bUsesBots)
		return;
	const UBetrayalGameInstance* Inst = GetGameInstance<UBetrayalGameInstance>();
	if (!Inst)
		return;
	
	const int32 PlayerCount = UGameplayStatics::GetNumPlayerStates(GetWorld());
	const FVector Location = UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetSpawnLocation();
	const FRotator Rotation = FRotator::ZeroRotator;
	
	FActorSpawnParameters SpawnParams;
	for (int i = 0; i < Inst->GetSubsystem<UBetrayalGameNetworkSubsystem>()->MAX_PLAYERS - PlayerCount; ++i)
	{
		if (AAIPlayerController* AIPlayerController = GetWorld()->SpawnActor<AAIPlayerController>(BotController, Location, Rotation))
		{;
			RestartPlayer(AIPlayerController);			
			AIPlayerController->EnableAIPlayer();
		}
	}

	TArray<AActor*> AIPlayerControllers;	
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AAIPlayerController::StaticClass(), AIPlayerControllers);

	for (AActor* Controller : AIPlayerControllers)
	{
		if (AAIPlayerController* Cont = Cast<AAIPlayerController>(Controller))
			Cont->EnableAIPlayer();
	}
}

void ABetrayalGameMode::EnableAIPlayerHauntMode()
{
	if (!bUsesBots)
		return;
	TArray<AActor*> AIPlayerControllers;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AAIPlayerController::StaticClass(), AIPlayerControllers);
	for (AActor* ControllerActor : AIPlayerControllers)
	{
		if (AAIPlayerController* AIPlayerController = Cast<AAIPlayerController>(ControllerActor))
			AIPlayerController->SetHauntBehaviours();
	}
}

void ABetrayalGameMode::ReplacePlayer(const ABetrayalPlayerController* BetrayalPlayerController) const
{
	// Spawn replacement bot if bBots is true.
	if (!bUsesBots)
		return;
	
	if (!BetrayalPlayerController)
	{
		UE_LOG(LogGameMode, Warning, TEXT("Replace Player Aborted: Can't find betrayal player controller."));
		return;
	}
		
	const FVector Location = FVector::Zero();
	const FRotator Rotation = FRotator::ZeroRotator;
	if (AAIPlayerController* AIPlayerController = GetWorld()->SpawnActor<AAIPlayerController>(BotController, Location, Rotation))
	{
		//GetWorld()->GetAuthGameMode()->RestartPlayerAtTransform(AIPlayerController, BetrayalPlayerController->DestroyedTransform);
		
		const ABetrayalPlayerState* LeavingPlayerState = BetrayalPlayerController->GetPlayerState<ABetrayalPlayerState>();
		ABetrayalPlayerState* AIState = AIPlayerController->GetPlayerState<ABetrayalPlayerState>();
		if (!LeavingPlayerState || !AIState)
		{
			UE_LOG(LogGameMode, Warning, TEXT("Replace Player Aborted: Can't find betrayal player states."));
			return;			
		}
		const APlayerCharacter* LeavingCharacter = LeavingPlayerState->GetControlledCharacter();
		if (!LeavingCharacter)
		{
			UE_LOG(LogGameMode, Warning, TEXT("Replace Player Aborted: Can't find controlled betrayal player character."));
			return;						
		}

		FActorSpawnParameters SpawnParams;
		APlayerCharacter* BotCharacter = GetWorld()->SpawnActor<APlayerCharacter>(LeavingCharacter->StaticClass(), BetrayalPlayerController->DestroyedTransform, SpawnParams);
		AIPlayerController->Possess(BotCharacter);
		AIPlayerController->EnableAIPlayer();		

		// Attempts to imitate the leaving players role.
		AIState->SetIsABot(true);
		AIState->SetIsTraitor(LeavingPlayerState->IsTraitor());
		// if (LeavingCharacter)
		// {
		// 	AIState->ChangeCharacter(LeavingCharacter->StaticClass());
		// 	GEngine->AddOnScreenDebugMessage(-10, 10.0f, FColor::Green, "Character: " + LeavingCharacter->GetName());			
		// }
		// else
		// {
		// 	GEngine->AddOnScreenDebugMessage(-10, 10.0f, FColor::Red, "Could not change character, is null");
		// }
		AIState->SetControlState(CS_AI);
	}
}

void ABetrayalGameMode::ReplaceBot(const ABetrayalPlayerController* BetrayalPlayerController) const
{
	if (!BetrayalPlayerController)
	{
		UE_LOG(LogGameMode, Warning, TEXT("Replace Bot Aborted: Can't find betrayal player controller."));
		return;
	}
	TArray<AActor*> BotControllers;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), BotController, BotControllers);
	if (BotControllers.Num() == 0)
		return;
	AAIPlayerController* AICont = Cast<AAIPlayerController>(BotControllers[0]);
	if (!AICont)
		return;
	APawn* Character = BetrayalPlayerController->GetPawn();
	if (!Character)
		return;

	// Places player at old bots last location.
	const FTransform Transform = AICont->GetPawn()->GetActorTransform();
	Character->SetActorTransform(Transform);

	// Attempts to replace if traitor or not.
	ABetrayalPlayerState* JoiningPlayerState = BetrayalPlayerController->GetPlayerState<ABetrayalPlayerState>();
	const ABetrayalPlayerState* AIState = AICont->GetPlayerState<ABetrayalPlayerState>();
	if (!JoiningPlayerState || !AIState)
	{
		UE_LOG(LogGameMode, Warning, TEXT("Replace Bot Aborted: Can't find betrayal player states."));
		//GEngine->AddOnScreenDebugMessage(-10, 10.0f, FColor::Red, "Player States Missing");
	}
	else
	{
		JoiningPlayerState->SetIsTraitor(AIState->IsTraitor());		
		//GEngine->AddOnScreenDebugMessage(-10, 10.0f, FColor::Red, JoiningPlayerState->GetName() + " is traitor: " + FString::FromInt(JoiningPlayerState->IsTraitor()));
	}

	// Destroys bot.
	AICont->GetPawn()->Destroy();
	AICont->Destroy();
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

TArray<APlayerStart*> ABetrayalGameMode::GetAllSpawnPoints() const
{
	TArray<APlayerStart*> OutSpawnPoints;
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), FoundActors);
	for (AActor* Actor : FoundActors)
	{
		OutSpawnPoints.Add(Cast<APlayerStart>(Actor));
	}
	return OutSpawnPoints;
}

APlayerStart* ABetrayalGameMode::GetRandomSpawnPoint() const
{
	TArray<APlayerStart*> AllSpawnPoints = GetAllSpawnPoints();
	if (AllSpawnPoints.Num() > 0)
	{
		const int RandomIndex = FMath::RandRange(0, AllSpawnPoints.Num() - 1);
		return AllSpawnPoints[RandomIndex];
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
		MaxStageTimer = HauntStage.TimeLength;		
		SetStageUseTimer(HauntStage.bUsesTimer);
		BetrayalGameState->OnMatchStageChanged(MatchStage);
		EnableAIPlayerHauntMode();
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
