// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_GetScaledDirection.h"

#include "AIController.h"
#include "NavigationSystem.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Vector.h"

UBTTask_GetScaledDirection::UBTTask_GetScaledDirection(FObjectInitializer const& ObjectInitializer)
{
	NodeName = TEXT("Get Scaled Direction");
	BlackboardKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UBTTask_GetScaledDirection, BlackboardKey), AActor::StaticClass());
	BlackboardKey.AddVectorFilter(this, GET_MEMBER_NAME_CHECKED(UBTTask_GetScaledDirection, BlackboardKey));
	ResultKey.AddVectorFilter(this, GET_MEMBER_NAME_CHECKED(UBTTask_GetScaledDirection, BlackboardKey));
}

EBTNodeResult::Type UBTTask_GetScaledDirection::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (!Controller)
		Controller = OwnerComp.GetAIOwner();
	if (!Pawn)
		Pawn = Controller->GetPawn();
	if (!World)
		World = GetWorld();
	if (!Blackboard)
		Blackboard = OwnerComp.GetBlackboardComponent();

	if (!Controller || !Pawn)
		return EBTNodeResult::Failed;
	
	FVector KeyVectorValue = FVector::Zero();
	if (BlackboardKey.SelectedKeyType == UBlackboardKeyType_Object::StaticClass())
	{
		UObject* KeyValue = Blackboard->GetValue<UBlackboardKeyType_Object>(BlackboardKey.GetSelectedKeyID());
		if (const AActor* TargetActor = Cast<AActor>(KeyValue))
		{
			KeyVectorValue = TargetActor->GetActorLocation();
		}
		else
		{
			UE_VLOG(Controller, LogBehaviorTree, Warning, TEXT("UBTTask_GetScaledDirection::ExecuteTask tried to go to actor while BB %s entry was empty"), *BlackboardKey.SelectedKeyName.ToString());
		}
	}
	else
	{
		KeyVectorValue = Blackboard->GetValue<UBlackboardKeyType_Vector>(BlackboardKey.GetSelectedKeyID());
	}
	
	FVector Destination = (KeyVectorValue - Pawn->GetActorLocation());
	Destination.Normalize();
	Destination *= Scale;

	if (bUseNavigation)
	{
		const UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(World);
	
		if (World && NavSys)
		{
			FNavLocation NavPos;
			if (NavSys->GetRandomReachablePointInRadius(Destination, NavigationGoalRadius, NavPos))
			{
				Destination = NavPos.Location;
			}
			else
			{
				return EBTNodeResult::InProgress;
			}
		}
	}

	Blackboard->SetValue<UBlackboardKeyType_Vector>(ResultKey.GetSelectedKeyID(), Destination);
	
	return EBTNodeResult::Succeeded;
}
