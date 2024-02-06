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
	
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
#pragma region Movement
	// Variables
private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monster | Movement", meta = (AllowPrivateAccess = "true"))
	float MoveSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monster | Movement", meta = (AllowPrivateAccess = "true"))
	float SprintSpeed;
	UPROPERTY(VisibleAnywhere, Replicated, BlueprintReadOnly, Category = "Monster | Movement", meta = (AllowPrivateAccess = "true"))
	bool bSprinting;
	
	// Getters
public:	
	float GetMoveSpeed() const { return MoveSpeed; }	
	float GetSprintSpeed() const { return SprintSpeed; }	
	bool IsSprinting() const { return bSprinting; }	
	void SetSprinting(const bool& IsSprinting);	
	
public:
	UPROPERTY()
	UCharacterMovementComponent* Movement;
	
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