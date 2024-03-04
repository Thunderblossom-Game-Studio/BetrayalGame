// Fill out your copyright notice in the Description page of Project Settings.

#include "BaseHaunt.h"

#include "ObjectivesComponent.h"
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
	FDataTableRowHandle NewSurvivorObjective)
:HauntName(NewName), HauntDescription(NewDescription),
 HauntCategory(NewCategory), bHasTimer(bUsesTimer),
 HauntDuration(NewDuration), bHasTraitor(bUsesTraitor),
 TraitorObjective(NewTraitorObjective), TraitorMonsters(NewTraitorMonsters),
SurvivorObjective(NewSurvivorObjective)
{
	GameState = nullptr;
	Traitor = nullptr;
}

void UBaseHaunt::StartHaunt()
{
	TraitorSetup();
		
	SurvivorSetup();
	
	OnHauntStart();
	
}

void UBaseHaunt::EndHaunt()
{
	OnHauntEnd();
}

void UBaseHaunt::TraitorSetup() const
{
	if(!bHasTraitor)
		return;
	
	ABetrayalPlayerState* RandomPlayer = GameState->GetRandomPlayer();
	RandomPlayer->SetIsTraitor(true);

	const FObjective* TraitorObjectiveData = TraitorObjective.DataTable->FindRow<FObjective>(TraitorObjective.RowName, "");
	if(!TraitorObjectiveData)
		return;

	const APlayerCharacter* TraitorCharacter = Cast<APlayerCharacter>(RandomPlayer->GetPawn());
	if(!TraitorCharacter)
		return;

	TraitorCharacter->ObjectivesComponent->Server_SetHauntObjective(*TraitorObjectiveData);
}

void UBaseHaunt::SurvivorSetup() const
{
	for (const auto Player : GameState->GetAllPlayers())
	{
		if(Player->IsTraitor())
			continue;
		
		const APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(Player->GetPawn());
		if(!PlayerCharacter)
			continue;

		const FObjective* SurvivorObjectiveData = SurvivorObjective.DataTable->FindRow<FObjective>(SurvivorObjective.RowName, "");
		if(!SurvivorObjectiveData)
			continue;

		PlayerCharacter->ObjectivesComponent->Server_SetHauntObjective(*SurvivorObjectiveData);
	}
}



