// Fill out your copyright notice in the Description page of Project Settings.


#include "../Gameplay/BaseInteractable.h"

#include "BaseCharacter.h"



// Sets default values
ABaseInteractable::ABaseInteractable()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	SetReplicates(true);

	Tags.Add("Interactable");
}

void ABaseInteractable::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	
}

void ABaseInteractable::OnInteract(ABaseCharacter* Interactor)
{
	if(HasAuthority())
		ServerOnInteract(Interactor);
	else
	{
		ServerOnInteract(Interactor);
		GEngine->AddOnScreenDebugMessage(-10, 3.0f, FColor::Red, Interactor->GetActorLabel() + " Interacted with " + GetActorLabel());
	}
	
}

bool ABaseInteractable::ServerOnInteract_Validate(ABaseCharacter* Interactor)
{
	return true;
}

void ABaseInteractable::ServerOnInteract_Implementation(ABaseCharacter* Interactor)
{
	Destroy();
}

void ABaseInteractable::OnBeginFocus(ABaseCharacter* Interactor)
{
}

void ABaseInteractable::OnEndFocus(ABaseCharacter* Interactor)
{
}

// Called when the game starts or when spawned
void ABaseInteractable::BeginPlay()
{
	Super::BeginPlay();

	
}

// Called every frame
void ABaseInteractable::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

