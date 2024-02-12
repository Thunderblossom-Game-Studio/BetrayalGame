// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Monster.h"
#include "Chaser.generated.h"

/**
 * 
 */
UCLASS()
class BETRAYALGAME_API AChaser : public AMonster
{
	GENERATED_BODY()

public:
	AChaser();

	virtual void BeginPlay() override;
		
#pragma region Movement	
// Getters
public:	
	void SetChasing(const bool& IsChasing);	
	
private:
	UPROPERTY()
	UCharacterMovementComponent* Movement;
	
#pragma endregion
	
#pragma region Attacking
// Variables
private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attacks | Teleporting", meta = (AllowPrivateAccess = "true"))
	FVector TeleportDestination;	

// Exposed Functions/Events
public:
	virtual void Attack(AActor* Target) override;
	UFUNCTION(BlueprintCallable)
	void Teleport(AActor* Target);
	UFUNCTION(BlueprintImplementableEvent)
	void OnTeleport(AActor* Target);

// Getters
public:	
	FVector GetTeleportDestination() const { return TeleportDestination; }	
	
#pragma endregion
};