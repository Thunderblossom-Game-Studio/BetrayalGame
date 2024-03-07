// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BaseCharacter.generated.h"


struct FInputActionValue;

UCLASS()
class BETRAYALGAME_API ABaseCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABaseCharacter();

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

#pragma region Debugging

	void NetDebugging();
	
#pragma endregion 
	
#pragma region Health System
	
protected:
	UPROPERTY(EditDefaultsOnly, Category = "Health")
	float MaxHealth;
	
	UPROPERTY(VisibleAnywhere, Replicated, Category = "Health")
	float CurrentHealth;

	UPROPERTY(VisibleAnywhere, Replicated, Category = "Health")
	bool bIsDead;

public:
	UFUNCTION(BlueprintPure, Category = "Health")
	float GetMaxHealth() const { return MaxHealth; }
	
	UFUNCTION(BlueprintPure, Category = "Health")
	float GetCurrentHealth() const { return CurrentHealth; }
	
	UFUNCTION()
	void SetMaxHealth(float NewMaxHealth) { MaxHealth = NewMaxHealth; }

	UFUNCTION(Server, Reliable)
	void Server_SetMaxHealth(float NewMaxHealth);

	UFUNCTION(BlueprintCallable)
	void TakeDamage(float Damage);

	UFUNCTION(Server, Reliable)
	void Server_TakeDamage(float Damage);

	UFUNCTION(BlueprintCallable)
	void Heal(float Amount);

	UFUNCTION(Server, Reliable)
	void Server_Heal(float Amount);

	UFUNCTION(BlueprintImplementableEvent, Category = "Health")
	void OnHeal(float Amount);
	
	UFUNCTION(BlueprintImplementableEvent, Category = "Health")
	void OnDamageTaken(float Damage);

	UFUNCTION(BlueprintPure, Category = "Health")
	bool IsDead() const { return bIsDead; }

	UFUNCTION(BlueprintImplementableEvent, Category = "Health")
	void OnDeath();
	
private:
#pragma endregion

#pragma region Movement

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
	float WalkSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
	float RunSpeed;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Movement")
	bool bIsRunning;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
	float StunnedSpeed;
	
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Movement")
	bool bIsStunned;

	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	float StunDuration;
	
public:
	// Different implementation depending if the character is AI or Player
	virtual void Move(const FInputActionValue& Value);
	virtual void Move(const FVector2D Value);

	
private:
#pragma endregion 
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
