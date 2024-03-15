// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_SearchForTargetActor.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/Engine.h"

UBTTask_SearchForTargetActor::UBTTask_SearchForTargetActor(FObjectInitializer const& ObjectInitializer) : Super(ObjectInitializer)
{
	NodeName = TEXT("Search For Target Actor");
	
	BlackboardKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UBTTask_SearchForTargetActor, BlackboardKey), AActor::StaticClass());
}

EBTNodeResult::Type UBTTask_SearchForTargetActor::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{	
	if (!Controller)
		Controller = OwnerComp.GetAIOwner();
	if (!Pawn)
		Pawn = Controller->GetPawn();
	if (!World)
		World = GetWorld();
	if (!Blackboard)
		Blackboard = OwnerComp.GetBlackboardComponent();
	
	TArray<AActor*> Targets;
	UGameplayStatics::GetAllActorsOfClass(World, ActorClass, Targets);
	
	if (Targets.Num() == 0)
		return EBTNodeResult::Failed;
	AActor* Target = Targets[0];

	if (bFindClosest)
	{
		float Distance = 0.0f;
		Target = UGameplayStatics::FindNearestActor(Pawn->GetActorLocation(), Targets, Distance);
	}

	Blackboard->SetValue<UBlackboardKeyType_Object>(BlackboardKey.GetSelectedKeyID(), Target);

	GEngine->AddOnScreenDebugMessage(1, 5.0f, FColor::Red, Target->GetFName().ToString());
	
	return EBTNodeResult::Succeeded;
}