// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTTask_MonsterAttack.generated.h"

/**
 * 
 */
UCLASS()
class BETRAYALGAME_API UBTTask_MonsterAttack : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
	
#pragma region Constructors
public:
	UBTTask_MonsterAttack(FObjectInitializer const& ObjectInitializer);
#pragma endregion
		
#pragma region Public Functions
public:
	/// Executes the main functionality of the task node.
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
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
	class AMonster* Monster;
#pragma endregion
};
