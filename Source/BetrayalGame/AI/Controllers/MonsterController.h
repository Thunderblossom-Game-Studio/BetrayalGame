// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
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

#pragma region Components
// Variables
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Components, meta = (AllowPrivateAccess = "true"))
	class UBehaviorTree* BehaviourTree;
	
// Getters
public:
	const UBehaviorTree* GetBehaviourTree() const { return BehaviourTree; }
	
#pragma endregion

#pragma region Perception Configuration
// Variables
protected:	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Perception | Sight", meta = (AllowPrivateAccess = "true"))
	class UAISenseConfig_Sight* SightConfig;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Perception | Sight", meta = (AllowPrivateAccess = "true"))
	float SightPermanenceTimer = 3.0f;
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FTimerHandle LOSTimerHandle;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Perception | Hearing", meta = (AllowPrivateAccess = "true"))
	class UAISenseConfig_Hearing* HearingConfig;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Perception | Hearing", meta = (AllowPrivateAccess = "true"))
	float HearingPermanenceTimer = 3.0f;
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FTimerHandle HearingTimerHandle;

// Getters
public:	
	const UAISenseConfig_Sight* GetSightConfig() const { return SightConfig; }
	const UAISenseConfig_Hearing* GetHearingConfig() const { return HearingConfig; }
	
#pragma endregion
		
#pragma region Movement
// Variables
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Monster | Movement", meta = (AllowPrivateAccess = "true"))
	TEnumAsByte<ERoomTraversalType> RoomTraversalType;

// Getters
public:
	ERoomTraversalType GetRoomTraversalType() const { return RoomTraversalType; }
	
#pragma endregion
	
#pragma region Targetting
// Variables
protected:	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Monster | Targetting", meta = (AllowPrivateAccess = "true"))
	AActor* TargetActor;

// Getters/Setters
public:
	const AActor* GetTargetActor() const { return TargetActor; }
	void SetTargetActor(AActor* NewTarget) { TargetActor = NewTarget; }
	
#pragma endregion
};