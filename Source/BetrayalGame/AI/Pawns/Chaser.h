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

#pragma region Movement
	// Variables
private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monster | Movement", meta = (AllowPrivateAccess = "true"))
	float MoveSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monster | Movement", meta = (AllowPrivateAccess = "true"))
	float SprintSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monster | Movement", meta = (AllowPrivateAccess = "true"))
	bool bSprinting;
	
	// Getters
public:	
	float GetMoveSpeed() const { return MoveSpeed; }	
	float GetSprintSpeed() const { return SprintSpeed; }	
	bool IsSprinting() const { return bSprinting; }	
	void SetSprinting(const bool& IsSprinting);	
	
#pragma endregion
	
#pragma region Catching
// Variables
private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monster | Attacks", meta = (AllowPrivateAccess = "true"))
	FVector TeleportDestination;	

// Exposed Functions/Events
public:
	UFUNCTION(BlueprintCallable)
	void Teleport(AActor* Target);
	UFUNCTION(BlueprintImplementableEvent)
	void OnTeleport(AActor* Target);

// Getters
public:	
	FVector GetTeleportDestination() const { return TeleportDestination; }	
	
#pragma endregion
};