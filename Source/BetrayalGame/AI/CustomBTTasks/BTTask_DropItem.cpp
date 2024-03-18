// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_DropItem.h"

#include "AIController.h"
#include "BetrayalGame/Gameplay/Player/PlayerCharacter.h"

UBTTask_DropItem::UBTTask_DropItem(FObjectInitializer const& ObjectInitializer)
{
	NodeName = TEXT("Drop Item");
}

EBTNodeResult::Type UBTTask_DropItem::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{	
	const AAIController* Controller = OwnerComp.GetAIOwner();
	if (!Controller)
		return EBTNodeResult::Failed;
	APlayerCharacter* Character = Controller->GetPawn<APlayerCharacter>();
	if (!Character)
		return EBTNodeResult::Failed;
	GLog->Log(Character->GetName() + TEXT(" dropping item..."));
	return EBTNodeResult::Succeeded;
}
