// Fill out your copyright notice in the Description page of Project Settings.


#include "BetrayalPlayerController.h"
#include "BetrayalGame/AI/Controllers/AIPlayerController.h"
#include "GameFramework/GameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "BetrayalGame/BetrayalPlayerState.h"
#include "Net/UnrealNetwork.h"

void ABetrayalPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if(HasAuthority())
	{
		InitializeReferences();
	}
	else
	{
		Server_InitializeReferences();
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

	SetControlledCharacter(DefaultCharacterBlueprint.GetDefaultObject());
	BetrayalPlayerState->SetControlledCharacter(GetControlledCharacter());

	FTransform SpawnTransform = FTransform{FQuat::Identity, FVector::ZeroVector, FVector::OneVector };
	
	AActor* CharacterToPossess = GetWorld()->SpawnActor(ControlledCharacter->GetClass(), &SpawnTransform);
	APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(CharacterToPossess);
	
	Possess(PlayerCharacter);

	/*TODO - Player is being spawned at 0,0,0 temporarily. GameMode needs to have a reference to all spawn points and assign them to players
	  TODO - Input is not working when player spawns, might need to be handled in the controller instead of the character.
	 */
	
}

void ABetrayalPlayerController::Server_InitializeReferences_Implementation()
{
	InitializeReferences();
}

void ABetrayalPlayerController::Server_SetControlledCharacter_Implementation(APlayerCharacter* NewControlledCharacter)
{
	SetControlledCharacter(NewControlledCharacter);
}
