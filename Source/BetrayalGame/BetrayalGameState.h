// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "GameFramework/GameState.h"
#include "BetrayalGameState.generated.h"

UENUM()
enum EMatchStage
{
	Preparing,
	Exploring,
	Haunting,
	Finishing
};

/**
 * 
 */
UCLASS()
class BETRAYALGAME_API ABetrayalGameState : public AGameState
{
	GENERATED_BODY()

public:
	ABetrayalGameState();
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

#pragma region Match Stage Variable Replication
// Replicated Variables
protected:
	UPROPERTY(VisibleAnywhere, Replicated, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TEnumAsByte<EMatchStage> MatchStage = Preparing;
	UPROPERTY(VisibleAnywhere, Replicated, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	int Countdown = 30;

public:
	UFUNCTION(BlueprintCallable)
	bool IsValidCountdown() const { return (Countdown >= 0); }
	
// Getters/Setters
public:
	const TEnumAsByte<EMatchStage>& GetMatchStage() const { return MatchStage; }
	float GetCountdown() const { return Countdown; }
	void SetMatchStage(const TEnumAsByte<EMatchStage>& NewStage) { MatchStage = NewStage; }
	void SetCountdown(const int NewTime) { Countdown = NewTime; }
	
#pragma endregion
	
};