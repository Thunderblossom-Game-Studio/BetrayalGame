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
		
#pragma region Components
	// Variables
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player | Components", meta = (AllowPrivateAccess = "true"))
	UBehaviorTree* BehaviourTree;
	
	// Getters
public:
	const UBehaviorTree* GetBehaviourTree() const { return BehaviourTree; }
	
#pragma endregion
	
#pragma region Perception Configuration
	// Variables
protected:	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player | Perception", meta = (AllowPrivateAccess = "true"))
	class UAISenseConfig_Sight* SightConfig;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player | Perception", meta = (AllowPrivateAccess = "true"))
	class UAISenseConfig_Hearing* HearingConfig;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player | Perception", meta = (AllowPrivateAccess = "true"))
	class UAISenseConfig_Damage* DamageConfig;

	// Exposed Functions/Events
public:
	UFUNCTION()
	void OnSenseTargetUpdated(AActor* UpdatedActor, FAIStimulus Stimulus);
	
	// Getters
public:	
	const UAISenseConfig_Sight* GetSightConfig() const { return SightConfig; }
	const UAISenseConfig_Hearing* GetHearingConfig() const { return HearingConfig; }
	const UAISenseConfig_Damage* GetDamageConfig() const { return DamageConfig; }

	// Cached Variables
private:
	UPROPERTY()
	UWorld* World;
	
#pragma endregion
};
