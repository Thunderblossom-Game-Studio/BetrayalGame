// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_Interact.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"
#include "BetrayalGame/Gameplay/Interactables/BaseInteractable.h"
#include "BetrayalGame/Gameplay/Player/PlayerCharacter.h"

UBTTask_Interact::UBTTask_Interact(FObjectInitializer const& ObjectInitializer)
	: World(nullptr), Blackboard(nullptr), Controller(nullptr),	Character(nullptr)
{
	NodeName = TEXT("Interact");
	BlackboardKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UBTTask_Interact, BlackboardKey), AActor::StaticClass());
}

EBTNodeResult::Type UBTTask_Interact::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Controller = OwnerComp.GetAIOwner();
	Blackboard = Controller->GetBlackboardComponent();
	Character = Controller->GetPawn<APlayerCharacter>();
	if (!Blackboard || !Character)
		return EBTNodeResult::Failed;
	
	UObject* KeyValue = Blackboard->GetValue<UBlackboardKeyType_Object>(BlackboardKey.GetSelectedKeyID());
	if (ABaseInteractable* TargetActor = Cast<ABaseInteractable>(KeyValue))
	{
		Character->InteractableInFocus = TargetActor;
		Character->Server_Interact(Character, TargetActor);
		GLog->Log(Character->GetName() + ": used...");
		return EBTNodeResult::Succeeded;
	}
	
	return EBTNodeResult::Failed;
}