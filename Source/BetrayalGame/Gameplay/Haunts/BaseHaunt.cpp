// Fill out your copyright notice in the Description page of Project Settings.

#include "BaseHaunt.h"

#include "../../BetrayalGameMode.h"
#include "../Player/Player Components/ObjectivesComponent.h"
#include "BetrayalGame/BetrayalGameState.h"
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
	DOREPLIFETIME(ABaseHaunt, Traitor);
	DOREPLIFETIME(ABaseHaunt, Survivors);
	
}

ABaseHaunt::ABaseHaunt(): HauntCategory(Hc_Asymmetric), bHasTimer(false), Traitor(nullptr), bHasTraitor(false), GameState(nullptr)
{
	bReplicates = true;
}

void ABaseHaunt::ConfigureHaunt(FName NewName, const FText& NewDescription, TEnumAsByte<EHauntCategory> NewCategory,
	bool bUsesTimer, float NewDuration, bool bUsesTraitor, TArray<TSubclassOf<AMonster>> NewTraitorMonsters/*, const FDataTableRowHandle& NewTraitorObjective,
	, const FDataTableRowHandle& NewSurvivorObjective*/)
{
	HauntName = NewName;
	HauntDescription = NewDescription;
	HauntCategory = NewCategory;
	bHasTimer = bUsesTimer;
	HauntDuration = NewDuration;
	bHasTraitor = bUsesTraitor;
	// TraitorObjective = NewTraitorObjective;
	TraitorMonsters = NewTraitorMonsters;
	// SurvivorObjective = NewSurvivorObjective;
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

void ABaseHaunt::TraitorSetup()
{
	if(!bHasTraitor)
		return;

	const ABetrayalGameMode* BMode = Cast<ABetrayalGameMode>(GetWorld()->GetAuthGameMode());
	if(!BMode)
		return;
	
	ABetrayalPlayerState* RandomPlayer = BMode->GetRandomPlayer();
	RandomPlayer->SetIsTraitor(true);

	const FObjective* TraitorObjectiveData = TraitorObjective.DataTable->FindRow<FObjective>(TraitorObjective.RowName, "");
	if(!TraitorObjectiveData)
		return;

	const APlayerCharacter* TraitorCharacter = Cast<APlayerCharacter>(RandomPlayer->GetPawn());
	if(!TraitorCharacter)
		return;

	TraitorCharacter->ObjectivesComponent->Server_SetHauntObjective(*TraitorObjectiveData);
	
	// TODO Not working fix later
	Traitor = TraitorCharacter;
	
	OnTraitorChosen(TraitorCharacter);
	
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Traitor Setup"));
}

void ABaseHaunt::SurvivorSetup()
{
	const ABetrayalGameMode* BMode = Cast<ABetrayalGameMode>(GetWorld()->GetAuthGameMode());
	if(!BMode)
		return;

	for (const auto Player : BMode->GetAllPlayerStates())
	{
		if(Player->IsTraitor())
			continue;
		
		APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(Player->GetPawn());
		if(!PlayerCharacter)
			continue;
		
		const FObjective* SurvivorObjectiveData = SurvivorObjective.DataTable->FindRow<FObjective>(SurvivorObjective.RowName, "");
		if(!SurvivorObjectiveData)
			continue;

		PlayerCharacter->ObjectivesComponent->Server_SetHauntObjective(*SurvivorObjectiveData);

		// TODO Not working fix later
		Survivors.Add(PlayerCharacter);
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Survivor Added"));
	}

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Survivors Setup"));
}





