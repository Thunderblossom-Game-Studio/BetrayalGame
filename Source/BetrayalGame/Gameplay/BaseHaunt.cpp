// Fill out your copyright notice in the Description page of Project Settings.

#include "BaseHaunt.h"

#include "BetrayalGame/BetrayalGameState.h"
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

UBaseHaunt::UBaseHaunt(FName NewName, FText NewDescription, TEnumAsByte<EHauntCategory> NewCategory, bool bUsesTimer,
	float NewDuration, bool bUsesTraitor, FDataTableRowHandle NewTraitorObjective, TArray<AMonster*> NewTraitorMonsters,
	FDataTableRowHandle NewSurvivorObjective, ABetrayalGameState* NewGameState)
:HauntName(NewName), HauntDescription(NewDescription),
 HauntCategory(NewCategory), bHasTimer(bUsesTimer),
 HauntDuration(NewDuration), bHasTraitor(bUsesTraitor),
 TraitorObjective(NewTraitorObjective), // TODO FINISH
{
}

void UBaseHaunt::StartHaunt()
{
	GameState = Cast<ABetrayalGameState>(GetWorld()->GetGameState());
	if(!GameState)
		return;
		
	OnHauntStart();
	
}

void UBaseHaunt::EndHaunt()
{
	OnHauntEnd();
}


