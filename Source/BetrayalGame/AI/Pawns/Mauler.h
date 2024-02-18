// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Monster.h"
#include "Mauler.generated.h"

/**
 * 
 */
UCLASS()
class BETRAYALGAME_API AMauler : public AMonster
{
	GENERATED_BODY()

public:
	AMauler();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

#pragma region Components
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	class USphereComponent* SphereComponent;
	
#pragma endregion

#pragma region Movement
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	FVector AnchorPoint;
	
#pragma endregion
	
#pragma region Attacking	
public:	
	virtual void Attack(AActor* Target) override;

#pragma endregion

#pragma region Targetting	
protected:
	UPROPERTY()
	class APlayerCharacter* TargetCharacter;
	UPROPERTY()
	TArray<class APlayerCharacter*> ValidTargets;
	
public:
	UFUNCTION()
	class APlayerCharacter* FindClosestCharacter();
	
protected:
	UFUNCTION()
	void OverlapEnter(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OverlapExit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

public:
	class APlayerCharacter* GetTargetCharacter() const { return TargetCharacter; }
	
#pragma endregion
	
};
