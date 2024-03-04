// Fill out your copyright notice in the Description page of Project Settings.

#include "BaseHaunt.h"

#include "BetrayalGameMode.h"
#include "GameModeInfoCustomizer.h"
#include "ObjectivesComponent.h"
#include "BetrayalGame/BetrayalGameState.h"
#include "Factories/BlueprintFactory.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "Net/UnrealNetwork.h"

void ABaseHaunt::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	UObject::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABaseHaunt, HauntName);
	DOREPLIFETIME(ABaseHaunt, HauntDescription);
	DOREPLIFETIME(ABaseHaunt, HauntCategory);
	DOREPLIFETIME(ABaseHaunt, HauntDuration);
	DOREPLIFETIME(ABaseHaunt, TraitorObjective);
	DOREPLIFETIME(ABaseHaunt, TraitorMonsters);
	DOREPLIFETIME(ABaseHaunt, SurvivorObjective)
}

ABaseHaunt::ABaseHaunt(): HauntCategory(Hc_Asymmetric), bHasTimer(false), Traitor(nullptr), bHasTraitor(false), GameState(nullptr)
{
	bReplicates = true;
}

ABaseHaunt::ABaseHaunt(FName NewName, const FText& NewDescription, TEnumAsByte<EHauntCategory> NewCategory, bool bUsesTimer,
                       float NewDuration, bool bUsesTraitor, const FDataTableRowHandle& NewTraitorObjective, const TArray<AMonster*>& NewTraitorMonsters,
                       const FDataTableRowHandle& NewSurvivorObjective)
:HauntName(NewName), HauntDescription(NewDescription),
 HauntCategory(NewCategory), bHasTimer(bUsesTimer),
 HauntDuration(NewDuration), bHasTraitor(bUsesTraitor),
 TraitorObjective(NewTraitorObjective), TraitorMonsters(NewTraitorMonsters),
SurvivorObjective(NewSurvivorObjective)
{
	GameState = nullptr;
	Traitor = nullptr;
}

void ABaseHaunt::StartHaunt()
{
	TraitorSetup();
		
	SurvivorSetup();
	
	OnHauntStart();
	
}

void ABaseHaunt::Server_StartHaunt_Implementation()
{
	StartHaunt();
}

void ABaseHaunt::EndHaunt()
{
	OnHauntEnd();
}

void ABaseHaunt::Server_EndHaunt_Implementation()
{
	EndHaunt();
}

void ABaseHaunt::TraitorSetup() const
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

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Traitor Setup"));
}

void ABaseHaunt::SurvivorSetup() const
{
	const ABetrayalGameMode* BMode = Cast<ABetrayalGameMode>(GetWorld()->GetAuthGameMode());
	if(!BMode)
		return;

	for (const auto Player : BMode->GetAllPlayerStates())
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

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Survivors Setup"));
}



