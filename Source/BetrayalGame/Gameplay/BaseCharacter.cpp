// Fill out your copyright notice in the Description page of Project Settings.


#include "../Gameplay/BaseCharacter.h"

#include "Net/UnrealNetwork.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Components/CapsuleComponent.h"

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
	// This runs ONLY on client
	if(IsLocallyControlled())
	{
		if(CurrentHealth <= 0.0f)
			bIsDead = true;
		
	}

	// This runs ONLY on the server
	if(GetLocalRole() == ROLE_Authority)
	{
		
	}


	if(bIsDead)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, "You are dead!");
	}

	
	// Everything else runs on all machines
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


}

// Called every frame
void ABaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ABaseCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	
	

	//PlayerInputComponent->BindKey(EKeys::SpaceBar, IE_Pressed, this, &ABaseCharacter::DealDamage);
}

