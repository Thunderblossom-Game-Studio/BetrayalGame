// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_DropItem.generated.h"

/**
 * 
 */
UCLASS()
class BETRAYALGAME_API UBTTask_DropItem : public UBTTaskNode
{
	GENERATED_BODY()
	
#pragma region Constructors
public:
	UBTTask_DropItem(FObjectInitializer const& ObjectInitializer);
#pragma endregion

#pragma region Public Functions
public:
	/// Executes the main functionality of the task node.
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
#pragma endregion
};
