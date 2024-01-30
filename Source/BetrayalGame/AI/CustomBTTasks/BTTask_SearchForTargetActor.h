// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"
#include "BTTask_SearchForTargetActor.generated.h"

/**
 * Searches for a target actor class and sets it to a blackboard key.
 */
UCLASS()
class BETRAYALGAME_API UBTTask_SearchForTargetActor : public UBTTask_BlackboardBase
{
	GENERATED_BODY()

#pragma region Constructors
public:
	UBTTask_SearchForTargetActor(FObjectInitializer const& ObjectInitializer);
#pragma endregion

#pragma region Public Functions
public:
	/// Executes the main functionality of the task node.
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
#pragma endregion
	
#pragma region Editable Variables
private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Node, meta=(AllowPrivateAccess = "true"))
	/// The actor class being searched for.
	TSubclassOf<AActor> ActorClass;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Node, meta=(AllowPrivateAccess = "true"))
	/// If it should find the closest target object or not.
	bool bFindClosest;
#pragma endregion
	
#pragma region Cached Pointers
private:
	UPROPERTY()
	UWorld* World;
	UPROPERTY()
	UBlackboardComponent* Blackboard;
	UPROPERTY()
	AAIController* Controller;
	UPROPERTY()
	APawn* Pawn;
#pragma endregion
};