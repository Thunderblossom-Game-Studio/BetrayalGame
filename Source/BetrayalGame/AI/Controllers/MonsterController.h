// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "MonsterController.generated.h"

/**
 * The damage operation that should be applied on attack.
 */
UENUM()
enum ERoomTraversalType
{
	NONE,
	PATROL,
	FREEROAM,
};

/**
 * 
 */
UCLASS()
class BETRAYALGAME_API AMonsterController : public AAIController
{
	GENERATED_BODY()

public:
	AMonsterController();

	virtual void BeginPlay() override;
	
#pragma region Components
// Variables
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Monster | Components", meta = (AllowPrivateAccess = "true"))
	UBehaviorTreeComponent* BehaviourTreeComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Monster | Components", meta = (AllowPrivateAccess = "true"))
	UAISenseConfig_Sight* SightConfig;

// Getters
public:
	const UBehaviorTreeComponent* GetBehaviourTreeComponent() const { return BehaviourTreeComponent; }
	const UAISenseConfig_Sight* GetSightConfig() const { return SightConfig; }
	
#pragma endregion

#pragma region Movement
// Variables
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Monster | Movement", meta = (AllowPrivateAccess = "true"))
	TEnumAsByte<ERoomTraversalType> RoomTraversalType;

// Getters
public:
	ERoomTraversalType GetRoomTraversalType() const { return RoomTraversalType; }
	
#pragma endregion
	
#pragma region Targetting
// Variables
protected:	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Monster | Targetting", meta = (AllowPrivateAccess = "true"))
	APawn* TargetPawn;

// Getters/Setters
public:
	const APawn* GetTargetPawn() const { return TargetPawn; }
	void SetTargetPawn(APawn* NewTarget) { TargetPawn = NewTarget; }
	
#pragma endregion
};