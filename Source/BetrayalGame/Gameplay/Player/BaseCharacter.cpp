// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseCharacter.h"

#include "Net/UnrealNetwork.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values
ABaseCharacter::ABaseCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;
}

void ABaseCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABaseCharacter, CurrentHealth);
	DOREPLIFETIME(ABaseCharacter, bIsDead);
	DOREPLIFETIME(ABaseCharacter, bIsRunning);
}

void ABaseCharacter::NetDebugging()
{
	if(!Controller)
		return;
	
	if (Controller->IsLocalPlayerController())
	{
		// Execute debug code only for the local player
		if (HasAuthority())
		{
			GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Red, "You are the Server!");
			
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Cyan, "You are the Client!");
		}
	}
}

void ABaseCharacter::Move(const FInputActionValue& Value)
{
	
}

void ABaseCharacter::Move(const FVector2D Value)
{
	
}

void ABaseCharacter::TakeDamage(float Damage)
{
	CurrentHealth -= Damage;
	
	if(CurrentHealth <= 0.0f)
	{
		CurrentHealth = 0.0f;
		bIsDead = true;
		OnDeath();
		return;
	}
	
	OnDamageTaken(Damage);
}

void ABaseCharacter::Heal(float Amount)
{
	if(CurrentHealth + Amount > MaxHealth)
		CurrentHealth = MaxHealth;
	else
		CurrentHealth += Amount;

	OnHeal(Amount);
}

void ABaseCharacter::Server_Heal_Implementation(float Amount)
{
	Heal(Amount);
}

void ABaseCharacter::Server_TakeDamage_Implementation(float Damage)
{
	TakeDamage(Damage);
}

void ABaseCharacter::Server_SetMaxHealth_Implementation(float NewMaxHealth)
{
	SetMaxHealth(NewMaxHealth);
}

// Called when the game starts or when spawned
void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	CurrentHealth = MaxHealth;

	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;

}

// Called every frame
void ABaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	NetDebugging();
}

// Called to bind functionality to input
void ABaseCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
}

