// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "BetrayalGame/BetrayalGameState.h"
#include "BetrayalGameMode.generated.h"

/**
 * Holds information about a stage in the match
 */
USTRUCT(Blueprintable)
struct FStageInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stage | Time")
	bool bUsesTimer = true;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stage | Time")
	float TimeLength = 60;
};

/**
 * 
 */
UCLASS()
class BETRAYALGAME_API ABetrayalGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	ABetrayalGameMode();

#pragma region Game Mode Functions
public:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	
	virtual void StartMatch() override;
	virtual void EndMatch() override;
private:	
#pragma endregion 

#pragma region Match Stage Handling
// Exposed Variables
public:
	// Match Stage's Information
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stages", meta = (AllowPrivateAccess = true))
	FStageInfo LobbyStage;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stages", meta = (AllowPrivateAccess = true))
	FStageInfo ExploreStage;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stages", meta = (AllowPrivateAccess = true))
	FStageInfo HauntStage;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stages", meta = (AllowPrivateAccess = true))
	FStageInfo FinishStage;
protected:
	// Stage Functionality
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stages|Debug", meta = (AllowPrivateAccess = true))
	TEnumAsByte<EMatchStage> MatchStage = Lobby;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stages|Debug", meta = (AllowPrivateAccess = true))
	float MaxStageTimer = 30;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stages|Debug", meta = (AllowPrivateAccess = true))
	float StageTimer = 0;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stages|Debug", meta = (AllowPrivateAccess = true))
	bool bStageUsesTimer = true;

// Exposed Functions/Events
public:
	// Allows anything authoritative to change stage.
	UFUNCTION(BlueprintCallable)
	void SetNextStage();
	
// Class Functions
protected:
	void RunMatchTimer(float DeltaSeconds);
	void SetMatchStage(TEnumAsByte<EMatchStage> NewStage);
	void SetStageUseTimer(const bool bUseTimer);

public:
	const TEnumAsByte<EMatchStage>& GetMatchStage() const { return MatchStage; }
	
// Cached Variables
protected:
	UPROPERTY()
	ABetrayalGameState* BetrayalGameState;
	
#pragma endregion

#pragma region AI Players
public:
	UFUNCTION(BlueprintCallable)
	void EnableAIPlayerControllers();
	UFUNCTION(BlueprintCallable)
	void EnableAIPlayerHauntMode();
private:
#pragma endregion

#pragma region Players
public:
	UFUNCTION(BlueprintCallable)
	TArray<ABetrayalPlayerState*> GetAllPlayerStates() const;
	UFUNCTION(BlueprintCallable)
	TArray<APlayerCharacter*> GetAllPlayerCharacters() const;
	UFUNCTION(BlueprintCallable)
	ABetrayalPlayerState* GetRandomPlayer() const;
private:
#pragma endregion

#pragma region Haunt Handling
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Haunt")
	TSubclassOf<ABaseHaunt> HauntClass;
	
	UFUNCTION(BlueprintCallable)
	void SetupHaunt();

private:

#pragma endregion 
};
