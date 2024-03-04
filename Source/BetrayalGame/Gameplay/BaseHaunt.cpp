// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseHaunt.h"

#include "Factories/BlueprintFactory.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "Net/UnrealNetwork.h"

void UBaseHaunt::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	UObject::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UBaseHaunt, HauntName);
	DOREPLIFETIME(UBaseHaunt, HauntDescription);
	DOREPLIFETIME(UBaseHaunt, HauntCategory);
	DOREPLIFETIME(UBaseHaunt, HauntDuration);
	DOREPLIFETIME(UBaseHaunt, TraitorObjective);
	DOREPLIFETIME(UBaseHaunt, TraitorMonsters);
	DOREPLIFETIME(UBaseHaunt, SurvivorObjective)
}

void UBaseHaunt::StartHaunt()
{
	OnHauntStart();
}

void UBaseHaunt::EndHaunt()
{
	OnHauntEnd();
}


