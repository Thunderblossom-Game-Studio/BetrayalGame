// Fill out your copyright notice in the Description page of Project Settings.


#include "BetrayalPlayerController.h"

#include "BetrayalGame/BetrayalGameMode.h"
#include "BetrayalGame/AI/Controllers/AIPlayerController.h"
#include "GameFramework/GameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "BetrayalGame/BetrayalPlayerState.h"
#include "GameFramework/PlayerStart.h"
#include "Net/UnrealNetwork.h"

void ABetrayalPlayerController::BeginPlay()
{
	Super::BeginPlay();
	
	if(HasAuthority())
	{
		InitializeReferences();
		SpawnPlayerCharacter();
		
	}
	else
	{
		Server_InitializeReferences();
		Server_SpawnPlayerCharacter();
		
	}
	
}

void ABetrayalPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABetrayalPlayerController, ControlledCharacter);
	DOREPLIFETIME(ABetrayalPlayerController, BetrayalPlayerState);
}

void ABetrayalPlayerController::InitializeReferences()
{
	BetrayalPlayerState = Cast<ABetrayalPlayerState>(PlayerState);
	if(!IsValid(BetrayalPlayerState))
	{
		UE_LOG(LogTemp, Error, TEXT("BetrayalPlayerController::InitializeReferences - BetrayalPlayerState is not valid"));
		return;
	}

	if(!DefaultCharacterBlueprint)
	{
		UE_LOG(LogTemp, Error, TEXT("BetrayalPlayerController::InitializeReferences - DefaultCharacterBlueprint is not valid"));
		return;
	}

	//SetControlledCharacter(DefaultCharacterBlueprint.GetDefaultObject());
	BetrayalPlayerState->SetControlledCharacter(BetrayalPlayerState->DefaultCharacterBlueprint.GetDefaultObject());

	Server_OnReferenceInitialized();
}

void ABetrayalPlayerController::Server_OnReferenceInitialized_Implementation()
{
	OnReferenceInitialized(ControlledCharacter, BetrayalPlayerState);
}


void ABetrayalPlayerController::SpawnPlayerCharacter()
{
	if(GetPawn())
	{
		// if (GetPawn()->InputComponent)
		// {
		// 	GetPawn()->InputComponent->bBlockInput = true;
		// 	GetPawn()->InputComponent->ClearActionBindings();
		// 	GetPawn()->InputComponent->ClearAxisBindings();
		// 	UE_LOG(LogTemp, , TEXT("Original Pawn bindings cleared for: %s"), HasAuthority() ? TEXT("Server") : TEXT("Client"));
		// }
		
		GetPawn()->Destroy();
		UE_LOG(LogTemp, Error, TEXT("Original Pawn Destroyed for: %s"), HasAuthority() ? TEXT("Server") : TEXT("Client"));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("BetrayalPlayerController::SpawnPlayer - GetPawn is not valid | SpawnPlayerCharacter: %s"), HasAuthority() ? TEXT("Server") : TEXT("Client"));
		return;
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
	
	Possess(GetWorld()->SpawnActor<APlayerCharacter>(BetrayalPlayerState->GetControlledCharacter()->GetClass(), SpawnTransform, FActorSpawnParameters()));

	APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(GetPawn());
	// PlayerCharacter->EnableInput(this);
	// PlayerCharacter->SetBetrayalPlayerController(this);
	// PlayerCharacter->InputPriority = 0;
	
	UE_LOG(LogTemp, Error, TEXT("New character possessed for: %s"), HasAuthority() ? TEXT("Server") : TEXT("Client"));
	//log player character controller
	//UE_LOG( LogTemp, Error, TEXT("Player Character Controller: %s"), *PlayerCharacter->GetController()->GetName() );
	//TODO - Input is not working when player spawns, might need to be handled in the controller instead of the character.


}

void ABetrayalPlayerController::Server_SpawnPlayerCharacter_Implementation()
{
	SpawnPlayerCharacter();
}

void ABetrayalPlayerController::Server_InitializeReferences_Implementation()
{
	InitializeReferences();
}

void ABetrayalPlayerController::Server_SetControlledCharacter_Implementation(APlayerCharacter* NewControlledCharacter)
{
	SetControlledCharacter(NewControlledCharacter);
}
