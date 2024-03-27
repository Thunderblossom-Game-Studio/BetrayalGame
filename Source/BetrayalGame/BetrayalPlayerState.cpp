// Fill out your copyright notice in the Description page of Project Settings.


#include "BetrayalPlayerState.h"

#include "BetrayalGameInstance.h"
#include "GameFramework/GameModeBase.h"
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
			ControlledCharacter = BetrayalPlayerState->ControlledCharacter;
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
			ControlledCharacter = BetrayalPlayerState->ControlledCharacter;
			ControlState = BetrayalPlayerState->ControlState;
		}
	}
}

void ABetrayalPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABetrayalPlayerState, bIsTraitor);
	DOREPLIFETIME(ABetrayalPlayerState, ControlledCharacter);
	DOREPLIFETIME(ABetrayalPlayerState, ControlState);
}

void ABetrayalPlayerState::Server_ChangeCharacter_Implementation(TSubclassOf<APlayerCharacter> NewControlledCharacter)
{
	ChangeCharacter(NewControlledCharacter);
}

void ABetrayalPlayerState::Server_SetControlledCharacter_Implementation(APlayerCharacter* NewControlledCharacter)
{
	SetControlledCharacter(NewControlledCharacter);
}

void ABetrayalPlayerState::SelectCharacter(TSubclassOf<APlayerCharacter> NewControlledCharacter)
{
	if(HasAuthority())
	{
		SetControlledCharacter(NewControlledCharacter.GetDefaultObject());
	}
	else
	{
		Server_SetControlledCharacter(NewControlledCharacter.GetDefaultObject());
	}
}

void ABetrayalPlayerState::ChangeCharacter(TSubclassOf<APlayerCharacter> NewControlledCharacter)
{
	GEngine->AddOnScreenDebugMessage(-10, 10.0f, FColor::Blue, "Attempting to change character");
	APawn* PreviousCharacter = GetOwningController()->GetPawn();
	if(!PreviousCharacter)
	{
		UE_LOG(LogTemp, Warning, TEXT("No previous character"));
		return;
	}

	// Save the transform before destroying the previous character
	const FTransform Transform = PreviousCharacter->GetTransform();
	
	FActorSpawnParameters SpawnParameters;
	SpawnParameters.Owner = GetOwningController();
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	
	AActor* NewCharacter = GetWorld()->SpawnActor<APlayerCharacter>(NewControlledCharacter, Transform, SpawnParameters);
	GEngine->AddOnScreenDebugMessage(-10, 10.0f, FColor::Red, "Transform: " + Transform.ToString() );
	if(!NewCharacter)
	{
		UE_LOG(LogTemp, Warning, TEXT("No new character"));
		return;
	}

	APlayerCharacter* NewPlayerCharacter = Cast<APlayerCharacter>(NewCharacter);
	if(!NewPlayerCharacter)
	{
		UE_LOG(LogTemp, Warning, TEXT("New character is not a player character"));
		return;
	}
	
	PreviousCharacter->Destroy();
	
	SetControlledCharacter(NewPlayerCharacter);
	
	GetOwningController()->Possess(NewPlayerCharacter);
}

void ABetrayalPlayerState::OnRep_IsTraitor()
{
	OnIsTraitorChanged(bIsTraitor);
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




