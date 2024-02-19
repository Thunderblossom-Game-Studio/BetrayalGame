// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MonsterController.h"
#include "MaulerController.generated.h"

UENUM()
enum EMaulerState { Evaluating, Standby, Acting };

/**
 * 
 */
UCLASS()
class BETRAYALGAME_API AMaulerController : public AMonsterController
{
	GENERATED_BODY()

public:
	AMaulerController();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	
#pragma region State Machine
// Variables
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", meta = (AllowPrivateAccess = "true"))
	TEnumAsByte<EMaulerState> MaulerState;

// State Functions
protected:
	virtual TEnumAsByte<EMaulerState> Evaluation();
	
	virtual void EvaluatingState();
	virtual void StandbyState();
	virtual void ActingState();

// Getters
public:
	const TEnumAsByte<EMaulerState>& GetMaulerState() const { return MaulerState; }
	
#pragma endregion

#pragma region Movement
// Variables
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	FVector AnchorPoint;

// Functions
protected:
	virtual void MoveAlongPath(class UNavigationPath* Path);
	
#pragma endregion

private:
	// Cached Variables
	UPROPERTY()
	class AMauler* Mauler;
	UPROPERTY()
	UWorld* World;
	UPROPERTY()
	class UNavigationSystemV1* NavigationSystem;
	UPROPERTY()
	FVector MovePosition;
	UPROPERTY()
	int PosIndex = 0;
};
