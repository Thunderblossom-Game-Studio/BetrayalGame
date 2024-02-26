// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "Gameplay/PlayerCharacter.h"
#include "BetrayalGameState.generated.h"

UENUM()
enum EMatchStage
{
	Lobby UMETA(DisplayName = "Lobby"),
	Exploring UMETA(DisplayName = "Exploring"),
	Haunting UMETA(DisplayName = "Haunting"),
	Finishing UMETA(DisplayName = "Finishing"),
};

UENUM()
enum EObjectiveState
{
	NotStarted UMETA(DisplayName = "Not Started"),
	Started UMETA(DisplayName = "Started"),
	Finished UMETA(DisplayName = "Finished")
};

UENUM()
enum EObjectiveType
{
	Event UMETA(DisplayName = "Event"),
	Innocent UMETA(DisplayName = "Innocent"),
	Traitor UMETA(DisplayName = "Traitor")
};

USTRUCT()
struct FObjective : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Objective")
	FString Name;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Objective")
	FString Description;

	UPROPERTY(BlueprintReadOnly, Category = "Objective")
	TEnumAsByte<EObjectiveState> State;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Objective")
	TEnumAsByte<EObjectiveType> Type;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Objective")
	bool bHasObjectiveItem;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Objective|Objective Item", meta = (EditCondition = "bHasObjectiveItem"))
	FDataTableRowHandle ObjectiveItem;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Objective|Objective Item", meta = (EditCondition = "bHasObjectiveItem"))
	int Amount;
};

USTRUCT()
struct FHaunt : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Haunt")
	FString Name;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Haunt")
	FString Description;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Haunt")
	FDataTableRowHandle TraitorObjective;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Haunt")
	FDataTableRowHandle InnocentObjective;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Haunt")
	float Duration;
};


UCLASS()
class BETRAYALGAME_API ABetrayalGameState : public AGameState
{
	GENERATED_BODY()

public:
	ABetrayalGameState();
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void BeginPlay() override;
	
#pragma region Match Stage Variable Replication
// Replicated Variables
protected:
	UPROPERTY(VisibleAnywhere, Replicated, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = "Game|Match Stage")
	TEnumAsByte<EMatchStage> MatchStage = Lobby;
	UPROPERTY(VisibleAnywhere, Replicated, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = "Game|Match Stage")
	int Countdown = 30;

	
	
public:
	UFUNCTION(BlueprintCallable)
	bool IsValidCountdown() const { return (Countdown >= 0); }

	UFUNCTION(NetMulticast, Reliable)
	void OnMatchStageChanged(const EMatchStage NewStage);
	UFUNCTION(BlueprintImplementableEvent)
	void OnLobbyStageStart();
	UFUNCTION(BlueprintImplementableEvent)
	void OnExploringStageStart();
	UFUNCTION(BlueprintImplementableEvent)
	void OnHauntingStageStart();
	DECLARE_EVENT(MatchStage, FHauntEvent)
	FHauntEvent& OnHaunt() { return HauntEvent; }
	FHauntEvent HauntEvent;
	
	UFUNCTION(BlueprintImplementableEvent)
	void OnFinishingStageStart();
	
// Getters/Setters
public:
	const TEnumAsByte<EMatchStage>& GetMatchStage() const { return MatchStage; }
	float GetCountdown() const { return Countdown; }
	void SetMatchStage(const TEnumAsByte<EMatchStage>& NewStage) { MatchStage = NewStage; }
	void SetCountdown(const int NewTime) { Countdown = NewTime; }
	
#pragma endregion

#pragma region Objectives
	//UFUNCTION(BlueprintCallable)
	//void GetCompletedObjectives(TArray<FObjective>& OutObjectives, const EObjectiveType Type);

	
	
#pragma endregion

#pragma region Haunt Tracking
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Game|Haunts")
	UDataTable* HauntData;
	
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Game|Haunts")
	FHaunt CurrentHaunt;
public:

	UFUNCTION()
	void InitializeHaunt();
	
	UFUNCTION()
	void StartHaunt();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Game|Haunts")
	void OnTraitorChosen(APlayerCharacter* Traitor);
#pragma endregion 
};