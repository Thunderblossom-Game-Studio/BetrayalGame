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

	bool SweepTraceForCharacter(ABaseCharacter*& HitCharacterOut);
	
#pragma region Debugging

	void NetDebugging();
	
#pragma endregion 
	
#pragma region Health System
	
protected:
	UPROPERTY(EditDefaultsOnly, Category = "Character|Health")
	float MaxHealth;
	
	UPROPERTY(VisibleAnywhere, Replicated, Category = "Character|Health")
	float CurrentHealth;

	UPROPERTY(VisibleAnywhere, Replicated, Category = "Character|Health")
	bool bIsDead;

public:
	UFUNCTION(BlueprintPure, Category = "Character|Health")
	float GetMaxHealth() const { return MaxHealth; }
	
	UFUNCTION(BlueprintPure, Category = "Character|Health")
	float GetCurrentHealth() const { return CurrentHealth; }
	
	UFUNCTION()
	void SetMaxHealth(float NewMaxHealth) { MaxHealth = NewMaxHealth; }

	UFUNCTION(Server, Reliable)
	void Server_SetMaxHealth(float NewMaxHealth);

	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
	
	UFUNCTION(BlueprintCallable)
	void TakeDamage(float Damage);
	
	UFUNCTION(Server, Reliable)
	void Server_TakeDamage(float Damage);
	
	UFUNCTION(BlueprintImplementableEvent, Category = "Character|Health")
	void OnDamageTaken(float Damage);
	
	UFUNCTION(BlueprintCallable)
	void Heal(float Amount);

	UFUNCTION(Server, Reliable)
	void Server_Heal(float Amount);

	UFUNCTION(BlueprintImplementableEvent, Category = "Character|Health")
	void OnHeal(float Amount);

	UFUNCTION(BlueprintPure, Category = "Character|Health")
	bool IsDead() const { return bIsDead; }

	UFUNCTION(BlueprintImplementableEvent, Category = "Character|Health")
	void OnDeath();
	
private:
#pragma endregion

#pragma region Movement

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character|Movement")
	float WalkSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character|Movement")
	float RunSpeed;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Character|Movement")
	bool bIsRunning;

public:
	// Different implementation depending if the character is AI or Player
	virtual void Move(const FInputActionValue& Value);
	virtual void Move(const FVector2D Value);

#pragma region Stun
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character|Combat|Stun")
	float StunnedSpeed;
	
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Character|Combat|Stun")
	bool bIsStunned;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character|Combat|Stun")
	float StunDuration;
		
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Character|Combat|Stun")
	FTimerHandle StunTimerHandle;
	
	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "Character|Combat|Stun")
	void StunAttack();
	
	UFUNCTION(BlueprintCallable, Category = "Character|Combat|Stun")
	void Stun(float Duration);

	UFUNCTION(Server, Reliable, Blueprintable, Category = "Character|Combat|Stun")
	void Server_Stun(float Duration);

	UFUNCTION(BlueprintCallable, Category = "Character|Combat|Stun")
	void StopStun();

	UFUNCTION(BlueprintImplementableEvent, Category = "Character|Combat|Stun")
	void OnStun();

	UFUNCTION(BlueprintImplementableEvent, Category = "Character|Combat|Stun")
	void OnStunEnd();
#pragma endregion
	
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
