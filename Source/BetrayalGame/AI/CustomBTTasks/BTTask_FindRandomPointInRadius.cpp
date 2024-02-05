// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_FindRandomPointInRadius.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Vector.h"
#include "NavigationSystem.h"

UBTTask_FindRandomPointInRadius::UBTTask_FindRandomPointInRadius(FObjectInitializer const& ObjectInitializer)
{
	NodeName = TEXT("Find Random Point In Radius");
	BlackboardKey.AddVectorFilter(this, GET_MEMBER_NAME_CHECKED(UBTTask_FindRandomPointInRadius, BlackboardKey));
}

EBTNodeResult::Type UBTTask_FindRandomPointInRadius::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (!Controller)
		Controller = OwnerComp.GetAIOwner();
	if (!Pawn)
		Pawn = Controller->GetPawn();
	if (!World)
		World = GetWorld();
	if (!Blackboard)
		Blackboard = OwnerComp.GetBlackboardComponent();
	
	FVector Destination = FVector::Zero();	
	const UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(World);
	
	if (World && NavSys)
	{
	    FNavLocation NavPos;
	    if (NavSys->GetRandomReachablePointInRadius(Pawn->GetActorLocation(), PatrolRadius, NavPos))
	    {
	        Destination = NavPos.Location;
	    }
	    else
	    {
	        return EBTNodeResult::InProgress;
	    }
	}	
	else
	{
		Blackboard->SetValue<UBlackboardKeyType_Vector>(BlackboardKey.GetSelectedKeyID(), Destination);
		return EBTNodeResult::Failed;
	}
	
	Blackboard->SetValue<UBlackboardKeyType_Vector>(BlackboardKey.GetSelectedKeyID(), Destination);
	
	return EBTNodeResult::Succeeded;
}
