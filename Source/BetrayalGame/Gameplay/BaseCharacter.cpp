// Fill out your copyright notice in the Description page of Project Settings.


#include "../Gameplay/BaseCharacter.h"

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
			
			const FString healthMessage = FString::Printf(TEXT("You have %f health remaining."), CurrentHealth);
			GEngine->AddOnScreenDebugMessage(-2, 0.0f, FColor::White, healthMessage);
			
			const FString speedMessage = FString::Printf(TEXT("Your current speed is: %f"),GetCharacterMovement()->MaxWalkSpeed);
			GEngine->AddOnScreenDebugMessage(-3, 0.0f, FColor::White, speedMessage);
			
			GEngine->AddOnScreenDebugMessage(-4, 0.0f, FColor::White, "Is Running: " + FString(bIsRunning ? "true" : "false"));
				
			GEngine->AddOnScreenDebugMessage(-5, 0.0f, FColor::White, "Is Dead: " + FString(bIsDead ? "true" : "false"));
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Cyan, "You are the Client!");

			const FString healthMessage = FString::Printf(TEXT("You have %f health remaining."), CurrentHealth);
			GEngine->AddOnScreenDebugMessage(-2, 0.0f, FColor::White, healthMessage);
			
			const FString speedMessage = FString::Printf(TEXT("Your current speed is: %f"),GetCharacterMovement()->MaxWalkSpeed);
			GEngine->AddOnScreenDebugMessage(-3, 0.0f, FColor::White, speedMessage);
			
			GEngine->AddOnScreenDebugMessage(-4, 0.0f, FColor::White, "Is Running: " + FString(bIsRunning ? "true" : "false"));
				
			GEngine->AddOnScreenDebugMessage(-5, 0.0f, FColor::White, "Is Dead: " + FString(bIsDead ? "true" : "false"));
		}
	}
}

void ABaseCharacter::Move(const FInputActionValue& Value)
{
	
}

void ABaseCharacter::Move(const FVector2D Value)
{
	
}

void ABaseCharacter::OnRep_CurrentHealth()
{
	OnHealthUpdate();
}

void ABaseCharacter::OnHealthUpdate()
{
	// Runs on every machine
	if(CurrentHealth <= 0.0f)
		bIsDead = true;
	
	
	if(IsLocallyControlled()) // Not sure if this is necessary
	{
		// Runs just on the server
		if(HasAuthority())
		{
			if(bIsDead)
				GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, "You are dead! and Server");
		}
		else
		{
			if(bIsDead)
				GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, "You are dead! and Client");
		}
	}
}

void ABaseCharacter::SetCurrentHealth(float NewCurrentHealth)
{
	if(GetLocalRole() == ROLE_Authority)
	{
		CurrentHealth = FMath::Clamp(NewCurrentHealth, 0.0f, MaxHealth);
		OnHealthUpdate();
	}
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

