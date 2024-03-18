// Fill out your copyright notice in the Description page of Project Settings.


#include "BetrayalPlayerState.h"

#include "BetrayalGameInstance.h"
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
		}
	}
}

void ABetrayalPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABetrayalPlayerState, bIsTraitor);
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
	//GetGameInstance<UBetrayalGameInstance>()->UpdatePlayerList();
	APlayerController* Ctrl = GetWorld()->GetFirstPlayerController();
	if (auto Menu = Cast<AMenu_PlayerController>(Ctrl))
	{
		Menu->UpdatePlayerList();
	}
}




