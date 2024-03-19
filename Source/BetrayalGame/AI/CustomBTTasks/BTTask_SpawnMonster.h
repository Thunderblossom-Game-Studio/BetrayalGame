// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_SpawnMonster.generated.h"

/**
 * 
 */
UCLASS()
class BETRAYALGAME_API UBTTask_SpawnMonster : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_SpawnMonster(FObjectInitializer const& ObjectInitializer);
	
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	
protected:	
	UPROPERTY(EditAnywhere, Category=Monster)
	TSubclassOf<class AMonster> MonsterType;
};
