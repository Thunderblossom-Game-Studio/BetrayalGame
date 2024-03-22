// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseInteractable.h"

#include "../Player/BaseCharacter.h"
#include "Net/UnrealNetwork.h"
#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "Perception/AISense_Sight.h"


// Sets default values
ABaseInteractable::ABaseInteractable()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	SetReplicates(true);
	//bAlwaysRelevant = true;
	
	AIStimuliSourceComponent = CreateDefaultSubobject<UAIPerceptionStimuliSourceComponent>(TEXT("AI Stimuli Source Component"));
	if (AIStimuliSourceComponent)
	{
		AIStimuliSourceComponent->bAutoRegister = true;
	}
}

void ABaseInteractable::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABaseInteractable, bIsInteractable);
	
}

void ABaseInteractable::OnInteract(AActor* Interactor)
{
	if(ActorHasTag("Door"))
		Destroy();
	
	InteractEvent(Interactor);
}

void ABaseInteractable::OnBeginFocus(AActor* Interactor)
{
}

void ABaseInteractable::OnEndFocus(AActor* Interactor)
{
}

// Called when the game starts or when spawned
void ABaseInteractable::BeginPlay()
{
	Super::BeginPlay();

	//GEngine->AddOnScreenDebugMessage(-10, 10.0f, FColor::Green, "Local Role for " + this->GetName() + " is: " + FString::FromInt(GetLocalRole()));
	
	//GEngine->AddOnScreenDebugMessage(-11, 10.0f, FColor::Green, "Remote Role for " + this->GetName() + " is: " + FString::FromInt(GetRemoteRole()));

}

void ABaseInteractable::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	
	if (AIStimuliSourceComponent)
	{		
		AIStimuliSourceComponent->RegisterForSense(UAISense_Sight::StaticClass());
		AIStimuliSourceComponent->RegisterWithPerceptionSystem();
	}
}

// Called every frame
void ABaseInteractable::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	

}

