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