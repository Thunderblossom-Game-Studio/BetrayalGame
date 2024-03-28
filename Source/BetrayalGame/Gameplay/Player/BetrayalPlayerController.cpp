// Fill out your copyright notice in the Description page of Project Settings.


#include "BetrayalPlayerController.h"

#include "BetrayalGame/BetrayalGameInstance.h"
#include "BetrayalGame/StaticUtils.h"
#include "BetrayalGame/BetrayalGameMode.h"
#include "BetrayalGame/AI/Controllers/AIPlayerController.h"
#include "GameFramework/GameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "BetrayalGame/BetrayalPlayerState.h"
#include "BetrayalGame/Lobby/BetrayalGameNetworkSubsystem.h"
#include "GameFramework/PlayerStart.h"
#include "Net/UnrealNetwork.h"

void ABetrayalPlayerController::BeginPlay()
{
	Super::BeginPlay();

	// Temp fix around no player state on clients.
	ACharacter* OldCharacter = GetPawn<ACharacter>();
	if (OldCharacter)
	{
		OldCharacter->GetMesh()->SetVisibility(false);		
	}
	
	FTimerHandle Handle;
	GetWorld()->GetTimerManager().SetTimer(Handle, this, &ABetrayalPlayerController::SetupPlayerCharacter, 0.3f);

	//SetupPlayerCharacter();
}

void ABetrayalPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(ABetrayalPlayerController, BetrayalPlayerState);
}

void ABetrayalPlayerController::PawnLeavingGame()
{
	if (!HasAuthority())
	{
		Super::PawnLeavingGame();
		return;
	}
	
	const ABetrayalGameMode* GameMode = GetWorld()->GetAuthGameMode<ABetrayalGameMode>();	
	if (!GameMode->UseBots())
	{
		Super::PawnLeavingGame();
		return;
	}
	
	ReplacePlayerWithBot();
}

void ABetrayalPlayerController::SetupPlayerCharacter()
{
	if (HasAuthority())
		DetermineNewOrReplaceCharacter();		
	else
		InitBetrayalPlayerState();
	
	SetupControllerInput();
}

void ABetrayalPlayerController::DetermineNewOrReplaceCharacter()
{
	UBetrayalGameNetworkSubsystem* NetworkSubsystem = GetGameInstance()->GetSubsystem<UBetrayalGameNetworkSubsystem>();
	if (!NetworkSubsystem)
	{
		UE_LOG(LogTemp, Error, TEXT("BetrayalPlayerController::DetermineNewOrReplaceCharacter: Network Subsystem is not valid"));
		return;		
	}
	
	TArray<AActor*> PlayerCharacters;
	
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerCharacter::StaticClass(), PlayerCharacters);
	
	UE_LOG(LogTemp, Display, TEXT("BetrayalPlayerController::BeginPlay - PlayerCharacters Count: %i"), PlayerCharacters.Num());
	
	if (PlayerCharacters.Num() >= NetworkSubsystem->MAX_PLAYERS)
	{
		ReplaceBotWithPlayer();
	}
	else
	{
		InitializeNewCharacter();
	}

	InitBetrayalPlayerState();
}

void ABetrayalPlayerController::SetupControllerInput()
{
	APlayerCharacter* PlayerCharacter = GetPawn<APlayerCharacter>();
	if (!PlayerCharacter)
	{
		UE_LOG(LogTemp, Error, TEXT("BetrayalPlayerController::SetupControllerInput: Couldn't find PlayerCharacter"));
		return;				
	}
	PlayerCharacter->SetupInputSubsystem();
}

void ABetrayalPlayerController::InitializeNewCharacter()
{
	Server_SpawnPlayerCharacter();
}

void ABetrayalPlayerController::InitBetrayalPlayerState()
{
	// if(!IsLocalController())
	// 	return;
	//
	
	BetrayalPlayerState = GetPlayerState<ABetrayalPlayerState>();
	if(BetrayalPlayerState)
	{
		PrintWarning("BetrayerPlayerController::InitBetrayalPlayerState - BetrayalPlayerState is valid");
	}
	else
	{
		PrintError("BetrayerPlayerController::InitBetrayalPlayerState - BetrayalPlayerState is not valid");
		return;
	}
	
	UBetrayalGameInstance* GameInstance = GetGameInstance<UBetrayalGameInstance>();
	if(!GameInstance)
	{
		PrintError("BetrayerPlayerController::InitBetrayalPlayerState - GameInstance is invalid");
	}
	
	if(GameInstance->CharacterBlueprints.Num() == 0)
	{
		PrintError("BetrayalPlayerController::SpawnPlayer - CharacterBlueprints is empty");
		return;
	}
	
	OnReferenceInitialized(GameInstance->CharacterBlueprints.Find(GameInstance->GetSelectedCharacter())->GetDefaultObject(), BetrayalPlayerState);
}

void ABetrayalPlayerController::Server_OnReferenceInitialized_Implementation(APlayerCharacter* ControlledPlayerCharacter, ABetrayalPlayerState* State)
{
	OnReferenceInitialized(ControlledPlayerCharacter, State);
}

void ABetrayalPlayerController::ReplacePlayerWithBot()
{
	APawn* ControlledPawn = GetPawn();
	if (!ControlledPawn)
	{
		UE_LOG(LogGameMode, Warning, TEXT("BetrayalPlayerController::ReplacePlayerWithBot: Failed to find controlled pawn."));
		return;
	}
	UnPossess();
	UWorld* World = GetWorld();
	if (!World)
	{
		UE_LOG(LogGameMode, Warning, TEXT("BetrayalPlayerController::ReplacePlayerWithBot: Failed to find world."));
		return;
	}
	
	const FVector Location = FVector::Zero();
	const FRotator Rotation = FRotator::ZeroRotator;
	if (AAIPlayerController* AIPlayerController = World->SpawnActor<AAIPlayerController>(BotController, Location, Rotation))
	{
		AIPlayerController->Possess(ControlledPawn);
		ABetrayalPlayerState* AIState = AIPlayerController->GetPlayerState<ABetrayalPlayerState>();

		if (ABetrayalGameMode* BetrayalGameMode = GetWorld()->GetAuthGameMode<ABetrayalGameMode>())
			BetrayalGameMode->UpdateAIPlayerMode(AIPlayerController);
		
		// Sets AI state to correct values.
		AIState->SetIsABot(true);
		AIState->SetControlState(CS_AI);
		UE_LOG(LogGameMode, Warning, TEXT("BetrayalPlayerController::ReplacePlayerWithBot: Success."));
	}
	else
	{		
		UE_LOG(LogGameMode, Error, TEXT("BetrayalPlayerController::ReplacePlayerWithBot: Couldn't spawn AI controller."));
	}
}

void ABetrayalPlayerController::ReplaceBotWithPlayer()
{
	if (!HasAuthority())
	{
		UE_LOG(LogTemp, Warning, TEXT("BetrayalPlayerController::ReplaceBotWithPlayer: Not Authorised"));
		return;
	}
	const UBetrayalGameNetworkSubsystem* NetworkSubsystem = GetGameInstance()->GetSubsystem<UBetrayalGameNetworkSubsystem>();
	if (!NetworkSubsystem)
	{
		UE_LOG(LogTemp, Error, TEXT("BetrayalPlayerController::ReplaceBotWithPlayer: Network Subsystem is not valid"));
		return;		
	}
	const int32 PlayerCount = UGameplayStatics::GetNumPlayerStates(GetWorld());
	ABetrayalPlayerState* State = nullptr;
	for (int32 i = 0; i < PlayerCount; ++i)
	{
		State = Cast<ABetrayalPlayerState>(UGameplayStatics::GetPlayerState(GetWorld(), i));
		if (!State)
			continue;
		if (State->IsABot())
			break;
	}
	if (!State)
	{
		UE_LOG(LogTemp, Warning, TEXT("BetrayalPlayerController::ReplaceBotWithPlayer: No available bot Player States"));
		return;			
	}

	AAIPlayerController* AICont = Cast<AAIPlayerController>(State->GetOwningController());
	if (!AICont)
	{
		UE_LOG(LogTemp, Warning, TEXT("BetrayalPlayerController::ReplaceBotWithPlayer: Invalid Player State, not actually a bot"));
		return;					
	}
	
	// Destroys bot.
	APawn* OldCharacter = GetPawn();
	if (OldCharacter)
	{
		UnPossess();
		OldCharacter->Destroy();
	}
	
	APawn* ControlledPawn = AICont->GetPawn();	
	AICont->UnPossess();
	Possess(ControlledPawn);
	AICont->Destroy();
	UE_LOG(LogTemp, Warning, TEXT("BetrayalPlayerController::ReplaceBotWithPlayer: Success."));
}

void ABetrayalPlayerController::SpawnPlayerCharacter()
{
	if(GetPawn())
	{
		GetPawn()->Destroy();
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("BetrayalPlayerController::SpawnPlayer - GetPawn is not valid | SpawnPlayerCharacter: %s"), HasAuthority() ? TEXT("Server") : TEXT("Client"));
	}
	
	// Get random spawn point from GameMode
	const ABetrayalGameMode* GameMode = Cast<ABetrayalGameMode>(GetWorld()->GetAuthGameMode());
	if(!GameMode)
	{
		UE_LOG(LogTemp, Error, TEXT("BetrayalPlayerController::SpawnPlayer - GameMode is not valid"));
		return;
	}

	const APlayerStart* SpawnPoint = GameMode->GetRandomSpawnPoint();
	if(!SpawnPoint)
	{
		UE_LOG(LogTemp, Error, TEXT("BetrayalPlayerController::SpawnPlayer - SpawnPoint is not valid"));
		return;
	}
	const FTransform SpawnTransform = SpawnPoint->GetTransform();


	UBetrayalGameInstance* GameInstance = GetGameInstance<UBetrayalGameInstance>();
	if(!GameInstance)
	{
		UE_LOG(LogTemp, Error, TEXT("BetrayalPlayerController::SpawnPlayer - GameInstance is not valid"));
		return;
	}

	if(GameInstance->CharacterBlueprints.Num() == 0)
	{
		UE_LOG(LogTemp, Error, TEXT("BetrayalPlayerController::SpawnPlayer - CharacterBlueprints is empty"));
		return;
	}

	APlayerCharacter* PlayerCharacter = GetWorld()->SpawnActor<APlayerCharacter>(GameInstance->CharacterBlueprints.Find(GameInstance->GetSelectedCharacter())->GetDefaultObject()->GetClass(), SpawnTransform, FActorSpawnParameters());
	
	Possess(PlayerCharacter);

	//Server_OnReferenceInitialized(BetrayalPlayerState->GetSelectedCharacter(), BetrayalPlayerState);
		
	PrintLog("BetrayalPlayerController::SpawnPlayer - PlayerCharacter spawned AND possessed");
	
}


void ABetrayalPlayerController::Server_SpawnPlayerCharacter_Implementation()
{
	SpawnPlayerCharacter();
}

void ABetrayalPlayerController::Server_InitializeReferences_Implementation()
{
	InitBetrayalPlayerState();
}