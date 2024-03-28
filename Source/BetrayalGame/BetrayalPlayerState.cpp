// Fill out your copyright notice in the Description page of Project Settings.


#include "BetrayalPlayerState.h"

#include "BetrayalGameInstance.h"
#include "StaticUtils.h"
#include "Lobby/Menu_PlayerController.h"
#include "Net/UnrealNetwork.h"

void ABetrayalPlayerState::CopyProperties(APlayerState* PlayerState)
{
	Super::CopyProperties(PlayerState);

	if(IsValid(PlayerState))
	{
		ABetrayalPlayerState* BetrayalPlayerState = Cast<ABetrayalPlayerState>(PlayerState);
		if(IsValid(BetrayalPlayerState))
		{
			bIsTraitor = BetrayalPlayerState->bIsTraitor;
			ControlState = BetrayalPlayerState->ControlState;
		}
	}
}

void ABetrayalPlayerState::OverrideWith(APlayerState* PlayerState)
{
	Super::OverrideWith(PlayerState);

	if(IsValid(PlayerState))
	{
		ABetrayalPlayerState* BetrayalPlayerState = Cast<ABetrayalPlayerState>(PlayerState);
		if(IsValid(BetrayalPlayerState))
		{
			bIsTraitor = BetrayalPlayerState->bIsTraitor;
			ControlState = BetrayalPlayerState->ControlState;
		}
	}
}

void ABetrayalPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABetrayalPlayerState, bIsTraitor);
	DOREPLIFETIME(ABetrayalPlayerState, ControlState);
}

void ABetrayalPlayerState::Server_ChangeCharacter_Implementation(TSubclassOf<APlayerCharacter> NewControlledCharacter)
{
	ChangeCharacter(NewControlledCharacter);
}

void ABetrayalPlayerState::ChangeCharacter(TSubclassOf<APlayerCharacter> NewControlledCharacter)
{
	// UE_LOG(LogTemp, Warning, TEXT("Attempting to change character"));
	// APawn* PreviousCharacter = GetOwningController()->GetPawn();
	// if(!PreviousCharacter)
	// {
	// 	UE_LOG(LogTemp, Warning, TEXT("No previous character"));
	// 	return;
	// }
	//
	// // Save the transform before destroying the previous character
	// const FTransform Transform = PreviousCharacter->GetTransform();
	//
	// // Destroy the previous character
	// PreviousCharacter->Destroy();
	//
	// // Setup spawn parameters for the new character
	// FActorSpawnParameters SpawnParameters;
	// SpawnParameters.Owner = GetOwningController();
	// SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	//
	//
	// UBetrayalGameInstance* GameInstance = GetWorld()->GetGameInstance<UBetrayalGameInstance>();
	// if(!GameInstance)
	// {
	// 	UE_LOG(LogTemp, Warning, TEXT("No GameInstance"));
	// 	return;
	// }
	//
	// APlayerCharacter* NewPlayerCharacter;
	//
	// switch (GameInstance->GetSelectedCharacter())
	// {
	// case C_Default:
	// 	NewPlayerCharacter = GetWorld()->SpawnActor<APlayerCharacter>(GameInstance->CharacterBlueprints.Find(GameInstance->GetSelectedCharacter())->GetDefaultObject()->GetClass(), Transform, SpawnParameters);
	// 	break;
	// case C_Heisenburger:
	// 	NewPlayerCharacter = GetWorld()->SpawnActor<APlayerCharacter>(HeisenburgerCharacterBlueprint, Transform, SpawnParameters);
	// 	break;
	// }
	//
	//
	// if(!NewPlayerCharacter)
	// {
	// 	UE_LOG(LogTemp, Warning, TEXT("No new character"));
	// 	return;
	// }
	//
	// SetSelectedCharacter(NewPlayerCharacter);
	//
	// GetOwningController()->Possess(NewPlayerCharacter);
}

void ABetrayalPlayerState::OnRep_IsTraitor()
{
	OnIsTraitorChanged(bIsTraitor);
}

void ABetrayalPlayerState::SetSelectedCharacter(TEnumAsByte<ECharacter> NewSelectedCharacter)
{
	UBetrayalGameInstance* GameInstance = GetWorld()->GetGameInstance<UBetrayalGameInstance>();
	if(!GameInstance)
	{
		PrintError("No GameInstance found");
		return;
	}

	GameInstance->SetSelectedCharacter(NewSelectedCharacter);
	
	OnSelectedCharacterChanged(NewSelectedCharacter);
	
	PrintWarning("FUNCTION: Character changed -> " + NewSelectedCharacter);
}


void ABetrayalPlayerState::SetIsReady_Implementation(bool bReady)
{
	bIsReady = bReady;
	Multicast_SetIsReady(bReady, this);
}

void ABetrayalPlayerState::Multicast_SetIsReady_Implementation(bool bReady, ABetrayalPlayerState* Player)
{
	Player->bIsReady = bReady;
	APlayerController* Ctrl = GetWorld()->GetFirstPlayerController();
	if (auto Menu = Cast<AMenu_PlayerController>(Ctrl))
	{
		Menu->UpdatePlayerList();
	}
}




