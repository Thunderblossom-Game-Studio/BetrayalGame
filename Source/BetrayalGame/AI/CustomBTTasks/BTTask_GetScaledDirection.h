// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTTask_GetScaledDirection.generated.h"

/**
 * 
 */
UCLASS()
class BETRAYALGAME_API UBTTask_GetScaledDirection : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
	
#pragma region Constructors
public:
	UBTTask_GetScaledDirection(FObjectInitializer const& ObjectInitializer);
	
#pragma endregion

#pragma region Editable Variables
private:
	UPROPERTY(EditAnywhere, Category=Blackboard)
	struct FBlackboardKeySelector ResultKey;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Node, meta=(AllowPrivateAccess = "true"))
	/// The radius size to find a point from.
	float Scale = 1;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Node, meta=(AllowPrivateAccess = "true"))
	/// The radius size to find a point from.
	bool bUseNavigation = false;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Node, meta=(AllowPrivateAccess = "true"))
	/// The radius size to find a point from.
	float NavigationGoalRadius = 50;
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
	class UBlackboardComponent* Blackboard;
	UPROPERTY()
	class AAIController* Controller;
	UPROPERTY()
	class APawn* Pawn;
#pragma endregion
};
