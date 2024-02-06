// Fill out your copyright notice in the Description page of Project Settings.


#include "../Gameplay/BaseInteractable.h"

#include "BaseCharacter.h"
#include "Net/UnrealNetwork.h"


// Sets default values
ABaseInteractable::ABaseInteractable()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	SetReplicates(true);
}

void ABaseInteractable::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABaseInteractable, InteractableOwner);
	
}

void ABaseInteractable::OnInteract(ABaseCharacter* Interactor)
{



	
}

void ABaseInteractable::ServerOnInteract_Implementation(ABaseCharacter* Interactor)
{
	NetMulticastOnInteract(Interactor);
}

void ABaseInteractable::NetMulticastOnInteract_Implementation(ABaseCharacter* Interactor)
{
	if(HasAuthority())
	{
		if(!GetOwner())
			return;
		
		GEngine->AddOnScreenDebugMessage(-10, 1.0f, FColor::Cyan, this->GetName() + " is now owned by " + GetOwner()->GetName());
	}
	else
	{
		if(!GetOwner())
			return;
		
		GEngine->AddOnScreenDebugMessage(-10, 1.0f, FColor::Purple, this->GetName() + " is now owned by " + GetOwner()->GetName());

	}
		
	// if(HasAuthority())
	// {
	// 	GEngine->AddOnScreenDebugMessage(-10, 1.0f, FColor::Purple, this->GetName() + " was destroyed by  " + Interactor->GetActorLabel());
	// 	//Destroy();
	// }
	// else
	// {
	// 	
	// 	Server_Destroy_Implementation(Interactor);
	// }
}


void ABaseInteractable::Server_Destroy_Implementation(class AActor* Who)
{
	GEngine->AddOnScreenDebugMessage(-10, 1.0f, FColor::Purple, this->GetName() + " was destroyed by  " + Who->GetActorLabel());
	
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
	
	
	if(!InteractableOwner)
		GEngine->AddOnScreenDebugMessage(-10, 1.0f, FColor::Red, "My name is " + this->GetName() + " and I have no owner");
	else
		GEngine->AddOnScreenDebugMessage(-10, 1.0f, FColor::Green, "My name is " + this->GetName() + " and my owner is " + InteractableOwner->GetName());


}

