// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_FindRandomPointInRadius.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Vector.h"
#include "NavigationSystem.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"
#include "BetrayalGame/AI/Pawns/Chaser.h"

UBTTask_FindRandomPointInRadius::UBTTask_FindRandomPointInRadius(FObjectInitializer const& ObjectInitializer)
{
	NodeName = TEXT("Find Random Point In Radius");
	BlackboardKey.AddVectorFilter(this, GET_MEMBER_NAME_CHECKED(UBTTask_FindRandomPointInRadius, BlackboardKey));
	OriginPointKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UBTTask_FindRandomPointInRadius, OriginPointKey), AActor::StaticClass());
	OriginPointKey.AddVectorFilter(this, GET_MEMBER_NAME_CHECKED(UBTTask_FindRandomPointInRadius, OriginPointKey));
}

EBTNodeResult::Type UBTTask_FindRandomPointInRadius::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Controller = OwnerComp.GetAIOwner();
	Pawn = Controller->GetPawn();
	World = GetWorld();
	Blackboard = OwnerComp.GetBlackboardComponent();
	
	FVector Destination = FVector::Zero();	
	const UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(World);

	FVector OriginPoint = FVector::Zero();
	if (Pawn)
		OriginPoint = Pawn->GetActorLocation();

	if (OriginPointKey.IsNone())
	{
		return EBTNodeResult::Failed;
	}	
	else if (OriginPointKey.SelectedKeyType == UBlackboardKeyType_Object::StaticClass())
	{
		UObject* KeyValue = Blackboard->GetValue<UBlackboardKeyType_Object>(OriginPointKey.GetSelectedKeyID());
		if (const AActor* TargetActor = Cast<AActor>(KeyValue))
		{
			OriginPoint = TargetActor->GetActorLocation();
		}
		else
		{
			UE_VLOG(Controller, LogBehaviorTree, Warning, TEXT("UBTTask_GetScaledDirection::ExecuteTask tried to go to actor while BB %s entry was empty"), *OriginPointKey.SelectedKeyName.ToString());
		}
	}
	else if (OriginPointKey.SelectedKeyType == UBlackboardKeyType_Vector::StaticClass())
	{
		OriginPoint = Blackboard->GetValue<UBlackboardKeyType_Vector>(OriginPointKey.GetSelectedKeyID());
	}

	Destination = OriginPoint;
	
	if (World && NavSys)
	{
	    FNavLocation NavPos;
	    if (NavSys->GetRandomReachablePointInRadius(OriginPoint, PatrolRadius, NavPos))
	        Destination = NavPos.Location;
	    else
	        return EBTNodeResult::Failed;
	}	
	else
	{
		return EBTNodeResult::Failed;
	}
	
	Blackboard->SetValue<UBlackboardKeyType_Vector>(BlackboardKey.GetSelectedKeyID(), Destination);
	
	return EBTNodeResult::Succeeded;
}
