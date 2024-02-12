// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MonsterController.h"
#include "ChaserController.generated.h"

/**
 * 
 */
UCLASS()
class BETRAYALGAME_API AChaserController : public AMonsterController
{
	GENERATED_BODY()

public:
	AChaserController();

	virtual void BeginPlay() override;
	
#pragma region Perception Configuration
// Exposed Functions/Events
public:
	UFUNCTION()
	void OnSenseTargetUpdated(AActor* UpdatedActor, FAIStimulus Stimulus);
	UFUNCTION(BlueprintImplementableEvent)
	void OnTargetFound(AActor* Target);
	UFUNCTION(BlueprintCallable)
	void LOSRecaptureFail();
	UFUNCTION(BlueprintImplementableEvent)
	void OnTargetLost(AActor* Target);
		
#pragma endregion
	
// Cached Variables
private:
	UPROPERTY()
	UWorld* World;
	UPROPERTY()
	class AChaser* ChaserPawn;
	
};