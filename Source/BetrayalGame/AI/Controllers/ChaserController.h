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
// Variables
protected:	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Monster | Perception", meta = (AllowPrivateAccess = "true"))
	UAISenseConfig_Sight* SightConfig;
	UPROPERTY(BlueprintReadOnly, Category = "Monster | Perception", meta = (AllowPrivateAccess = "true"))
	float LineOfSightTimer = 3.0f;
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FTimerHandle LOSTimerHandle;
	
public:
	UFUNCTION()
	void OnSenseTargetUpdated(AActor* UpdatedActor, FAIStimulus Stimulus);
	UFUNCTION(BlueprintCallable)
	void LOSRecaptureFail();
	
// Getters
public:	
	const UAISenseConfig_Sight* GetSightConfig() const { return SightConfig; }

private:
	UPROPERTY()
	UWorld* World;
	UPROPERTY()
	class AChaser* ChaserPawn;;
	
#pragma endregion
	
};