// Fill out your copyright notice in the Description page of Project Settings.


#include "BetrayalPlayerState.h"

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
	DOREPLIFETIME(ABetrayalPlayerState, bIsReady);
}

void ABetrayalPlayerState::OnRep_IsTraitor()
{
	OnIsTraitorChanged(bIsTraitor);
}

void ABetrayalPlayerState::OnRep_IsReady()
{
}

