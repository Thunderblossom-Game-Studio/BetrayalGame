// Fill out your copyright notice in the Description page of Project Settings.


#include "ChaserController.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "BetrayalGame/AI/Pawns/Chaser.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BetrayalGame/Gameplay/BetrayalPlayerState.h"
#include "BetrayalGame/Gameplay/PlayerCharacter.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "Engine/Engine.h"
#include "Perception/AIPerceptionComponent.h"

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

	World = GetWorld();
	ChaserPawn = GetPawn<AChaser>();

	if (ChaserPawn)
		ChaserPawn->SetChasing(false);
	
	if (GetLocalRole() == ROLE_Authority)
	{
		if (PerceptionComponent)
		{
			PerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &AChaserController::OnSenseTargetUpdated);
		}
		if (BehaviourTree)
		{
			RunBehaviorTree(BehaviourTree);			
		}
	}
}

void AChaserController::OnSenseTargetUpdated(AActor* UpdatedActor, FAIStimulus Stimulus)
{
	if (!UpdatedActor || !HasAuthority())
		return;	
	if (!ChaserPawn)
		ChaserPawn = GetPawn<AChaser>();
	
	if (Stimulus.WasSuccessfullySensed())
	{
		World->GetTimerManager().ClearTimer(LOSTimerHandle);

		if (!UpdatedActor->IsA(APlayerCharacter::StaticClass()))
			return;
		if (TargetActor)
			return;
		
		const APlayerCharacter* TargetCharacter = Cast<APlayerCharacter>(UpdatedActor);		
		const ABetrayalPlayerState* BetrayalPlayerState = TargetCharacter->GetPlayerState<ABetrayalPlayerState>();
		if (BetrayalPlayerState->IsTraitor())
			return;
		
		
		TargetActor = UpdatedActor;
		Blackboard->SetValueAsObject("TargetActor", TargetActor);
		OnTargetFound(TargetActor);
		
		//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, "Detected Actor: " + TargetActor->GetFName().ToString());
		Blackboard->SetValueAsBool("LineOfSight", true);
		if (ChaserPawn)
			ChaserPawn->SetChasing(true);
	}
	else if (TargetActor && TargetActor == UpdatedActor)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue, "Detected Actor: " + TargetActor->GetFName().ToString());
		FTimerDelegate LoseDelegate;
		LoseDelegate.BindUFunction(this, FName("LOSRecaptureFail"), UpdatedActor);
		World->GetTimerManager().SetTimer(LOSTimerHandle, LoseDelegate, SightPermanenceTimer, false);
	}
}

void AChaserController::LOSRecaptureFail()
{
	World->GetTimerManager().ClearTimer(LOSTimerHandle);	
	if (TargetActor)
		Blackboard->SetValueAsVector("LastKnownLocation", TargetActor->GetActorLocation());
	OnTargetLost(TargetActor);
	TargetActor = nullptr;	
	Blackboard->ClearValue("TargetActor");

	Blackboard->SetValueAsBool("LineOfSight", false);
	if (ChaserPawn)
		ChaserPawn->SetChasing(false);
}
