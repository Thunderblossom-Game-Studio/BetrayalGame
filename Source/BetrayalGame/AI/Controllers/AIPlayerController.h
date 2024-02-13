// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "AIPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class BETRAYALGAME_API AAIPlayerController : public AAIController
{
	GENERATED_BODY()
	
public:
	AAIPlayerController();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
		
#pragma region Components
// Variables
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Components, meta = (AllowPrivateAccess = "true"))
	UBehaviorTree* BehaviourTree;
	
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
	FTimerHandle SightTimerHandle;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Perception | Hearing", meta = (AllowPrivateAccess = "true"))
	class UAISenseConfig_Hearing* HearingConfig;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Perception | Hearing", meta = (AllowPrivateAccess = "true"))
	float HearingPermanenceTimer = 3.0f;
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FTimerHandle HearingTimerHandle;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Perception | Damage", meta = (AllowPrivateAccess = "true"))
	class UAISenseConfig_Damage* DamageConfig;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Perception | Damage", meta = (AllowPrivateAccess = "true"))
	float DamagePermanenceTimer = 3.0f;
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FTimerHandle DamageTimerHandle;

// Exposed Functions/Events
public:
	UFUNCTION()
	void OnSenseTargetUpdated(AActor* UpdatedActor, FAIStimulus Stimulus);
	
// Getters
public:	
	const UAISenseConfig_Sight* GetSightConfig() const { return SightConfig; }
	float GetSightPermanenceTimer() const { return SightPermanenceTimer; }
	const UAISenseConfig_Hearing* GetHearingConfig() const { return HearingConfig; }
	float GetHearingPermanenceTimer() const { return HearingPermanenceTimer; }
	const UAISenseConfig_Damage* GetDamageConfig() const { return DamageConfig; }
	float GetDamagePermanenceTimer() const { return DamagePermanenceTimer; }
	
#pragma endregion

#pragma region Line Of Sight
private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player | Perception | Sight", meta = (AllowPrivateAccess = "true"))
	class AMonster* AttackingMonster;
	UPROPERTY(BlueprintReadOnly, Category = "Player | Perception | Sight", meta = (AllowPrivateAccess = "true"))
	float LineOfSightTimer = 4.0f;
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FTimerHandle LOSTimerHandle;

	// Exposed Functions/Events
public:
	UFUNCTION(BlueprintCallable)
	void LOSRecaptureFail();

#pragma endregion
	
// Cached Variables
private:
	UPROPERTY()
	UWorld* World;	
	UPROPERTY()
	class ABetrayalGameMode* BetrayalGameMode;	
	UPROPERTY()
	class APlayerCharacter* PlayerCharacter;
	
};
