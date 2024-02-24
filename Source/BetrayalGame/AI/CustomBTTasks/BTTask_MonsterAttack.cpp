// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_MonsterAttack.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"
#include "BetrayalGame/AI/Pawns/Monster.h"

UBTTask_MonsterAttack::UBTTask_MonsterAttack(FObjectInitializer const& ObjectInitializer)
	: World(nullptr), Blackboard(nullptr), Controller(nullptr), Monster(nullptr)
{
	NodeName = TEXT("Monster Attack");
	BlackboardKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UBTTask_MonsterAttack, BlackboardKey), AActor::StaticClass());
}

EBTNodeResult::Type UBTTask_MonsterAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (!Controller)
		Controller = OwnerComp.GetAIOwner();
	if (!Monster)
		Monster = Controller->GetPawn<AMonster>();
	if (!Monster)
		return EBTNodeResult::Failed;
	
	UObject* KeyValue = Blackboard->GetValue<UBlackboardKeyType_Object>(BlackboardKey.GetSelectedKeyID());
	if (AActor* TargetActor = Cast<AActor>(KeyValue))
	{
		Monster->Attack(TargetActor);
		return EBTNodeResult::Succeeded;
	}	
	return EBTNodeResult::Failed;
}
