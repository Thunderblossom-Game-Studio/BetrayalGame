// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_SpawnMonster.h"

#include "AIController.h"
#include "BetrayalGame/AI/Pawns/Monster.h"
#include "BetrayalGame/Gameplay/Player/PlayerCharacter.h"

UBTTask_SpawnMonster::UBTTask_SpawnMonster(FObjectInitializer const& ObjectInitializer)
{
	NodeName = TEXT("Spawn Monster");
}

EBTNodeResult::Type UBTTask_SpawnMonster::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	const AAIController* Controller = OwnerComp.GetAIOwner();
	if (!Controller)
		return EBTNodeResult::Failed;
	APlayerCharacter* Character = Controller->GetPawn<APlayerCharacter>();
	if (!Character)
		return EBTNodeResult::Failed;
	Character->Server_SpawnMonster(MonsterType);
	return EBTNodeResult::Succeeded;
}
