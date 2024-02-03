// Fill out your copyright notice in the Description page of Project Settings.


#include "ChaserController.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "Engine/Engine.h"

AChaserController::AChaserController()
{
	PerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("Perception"));
	
	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("Sight Config"));
	if (SightConfig)
	{
		SightConfig->SightRadius = 1500;
		SightConfig->LoseSightRadius = 1800;
		SightConfig->PeripheralVisionAngleDegrees = 70.0f;
		SightConfig->AutoSuccessRangeFromLastSeenLocation = 500.0f;
		SightConfig->DetectionByAffiliation.bDetectEnemies = true;
		SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
		SightConfig->DetectionByAffiliation.bDetectFriendlies = true;
		SightConfig->SetMaxAge(5);
		PerceptionComponent->ConfigureSense(*SightConfig);
	}
}

void AChaserController::BeginPlay()
{
	Super::BeginPlay();

	if (GetLocalRole() == ROLE_Authority)
	{
		if (PerceptionComponent)			
			PerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &AChaserController::OnSenseTargetUpdated);
		if (BehaviourTree)
			RunBehaviorTree(BehaviourTree);
	}
}

void AChaserController::OnSenseTargetUpdated(AActor* UpdatedActor, FAIStimulus Stimulus)
{
	if (!UpdatedActor)
		return;
	
	if (Stimulus.WasSuccessfullySensed())
	{
		TargetActor = UpdatedActor;
		//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, "Detected Actor: " + TargetActor->GetFName().ToString());
		Blackboard->SetValueAsObject("TargetActor", TargetActor);
	}
	else if (TargetActor == UpdatedActor)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue, "Detected Actor: " + TargetActor->GetFName().ToString());
		Blackboard->SetValueAsVector("LastKnownLocation", TargetActor->GetActorLocation());
		TargetActor = nullptr;
		Blackboard->ClearValue("TargetActor");			
	}	
}
