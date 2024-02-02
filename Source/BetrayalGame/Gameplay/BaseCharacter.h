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
	
private:
	UPROPERTY(EditDefaultsOnly, Category = "Health")
	float MaxHealth;
	UPROPERTY(ReplicatedUsing = OnRep_CurrentHealth)
	float CurrentHealth;

	UPROPERTY(ReplicatedUsing = OnRep_CurrentHealth)
	bool bIsDead;

public:

	
	UFUNCTION()
	void OnRep_CurrentHealth(); // This runs when the CurrentHealth variable is updated on the server

	void OnHealthUpdate();
	
	UFUNCTION(BlueprintPure, Category = "Health")
	float GetMaxHealth() const { return MaxHealth; }

	//TODO: Implement this
	//UFUNCTION(BlueprintCallable, Category = "Health")
	//void SetMaxHealth(float NewMaxHealth);

	UFUNCTION(BlueprintPure, Category = "Health")
	float GetCurrentHealth() const { return CurrentHealth; }
	
	UFUNCTION(BlueprintCallable, Category = "Health")
	void SetCurrentHealth(float NewCurrentHealth);

	bool IsDead() const { return bIsDead; }

#pragma endregion


#pragma region Movement

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
	float WalkSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
	float RunSpeed;

	UPROPERTY(Replicated)
	bool bIsRunning;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
	float StunnedSpeed;
	
	UPROPERTY(Replicated)
	bool bIsStunned;

	// Different implementation depending if the character is AI or Player
	virtual void Move(const FInputActionValue& Value);
	virtual void Move(const FVector2D Value);
	
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
