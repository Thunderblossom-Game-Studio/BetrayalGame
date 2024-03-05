// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../../Gameplay/Player/BaseCharacter.h"
#include "Monster.generated.h"

/**
 * The damage operation that should be applied on attack.
 */
UENUM()
enum EDamageOperation
{
	SUBTRACT,
	DIVIDE,
	REPLACE,
};

/**
 * The base class for monsters to inherit from.
 */
UCLASS()
class BETRAYALGAME_API AMonster : public ABaseCharacter
{
	GENERATED_BODY()
	
public:
	AMonster();

#pragma region	Attacking
// Variables
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Monster | Attacks", meta = (AllowPrivateAccess = "true"))
	float AttackDamage;	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Monster | Attacks", meta = (AllowPrivateAccess = "true"))
	TEnumAsByte<EDamageOperation> AttackDamageOperation;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Monster | Attacks", meta = (AllowPrivateAccess = "true"))
	float AttackSpeed;	

// Exposed Functions/Events
public:
	UFUNCTION(BlueprintCallable)
	virtual void Attack(AActor* Target);
	UFUNCTION(BlueprintImplementableEvent)
	void OnAttack(AActor* Target);

// Getters/Setters
public:
	float GetAttackDamage() const { return AttackDamage; }
	void SetAttackDamage(float NewAttackDamage) { AttackDamage = NewAttackDamage; }	
	EDamageOperation GetAttackDamageOperation() const { return AttackDamageOperation; }
	void SetAttackDamageOperation(const EDamageOperation& NewAttackDamageOperation) { AttackDamageOperation = NewAttackDamageOperation; }	
	float GetAttackSpeed() const { return AttackSpeed; }
	void SetAttackSpeed(float NewAttackSpeed) { AttackSpeed = NewAttackSpeed; }
	
#pragma endregion
};
