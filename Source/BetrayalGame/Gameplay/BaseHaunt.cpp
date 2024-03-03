// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseHaunt.h"

#include "Factories/BlueprintFactory.h"
#include "Kismet2/KismetEditorUtilities.h"

void UBaseHaunt::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	UObject::GetLifetimeReplicatedProps(OutLifetimeProps);
}


void UBaseHaunt::StartHaunt()
{
	OnHauntStart();
}

void UBaseHaunt::OnTraitorPicked_Implementation(APlayerCharacter* TraitorPlayer)
{

}

void UBaseHaunt::OnHauntEnd_Implementation()
{
	
}
