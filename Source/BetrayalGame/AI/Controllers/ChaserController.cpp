// Fill out your copyright notice in the Description page of Project Settings.


#include "ChaserController.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "BetrayalGame/AI/Pawns/Chaser.h"
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

	World = GetWorld();
	ChaserPawn = GetPawn<AChaser>();

	if (ChaserPawn)
		ChaserPawn->SetSprinting(false);
	
	if (GetLocalRole() == ROLE_Authority)
	{
		if (PerceptionComponent)
		{
			PerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &AChaserController::OnSenseTargetUpdated);
		}
		if (BehaviourTree)
			RunBehaviorTree(BehaviourTree);
	}
}

void AChaserController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

void AChaserController::OnSenseTargetUpdated(AActor* UpdatedActor, FAIStimulus Stimulus)
{
	if (!UpdatedActor)
		return;	
	if (Stimulus.WasSuccessfullySensed())
	{
		World->GetTimerManager().ClearTimer(LOSTimerHandle);
		if (TargetActor)
		{
			const float CurrentTargetDistance = FVector::Distance(TargetActor->GetActorLocation(), GetPawn()->GetActorLocation());
			const float NewTargetDistance = FVector::Distance(UpdatedActor->GetActorLocation(), GetPawn()->GetActorLocation());
			if (NewTargetDistance < CurrentTargetDistance)
				TargetActor = UpdatedActor;
		}
		else
		{
			TargetActor = UpdatedActor;			
		}
		//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, "Detected Actor: " + TargetActor->GetFName().ToString());
		Blackboard->SetValueAsBool("LineOfSight", true);
		Blackboard->SetValueAsObject("TargetActor", TargetActor);
		if (ChaserPawn)
			ChaserPawn->SetSprinting(true);
	}
	else if (TargetActor && TargetActor == UpdatedActor)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue, "Detected Actor: " + TargetActor->GetFName().ToString());
		World->GetTimerManager().SetTimer(LOSTimerHandle, this, &AChaserController::LOSRecaptureFail, LineOfSightTimer, false);
	}
}

void AChaserController::LOSRecaptureFail()
{
	World->GetTimerManager().ClearTimer(LOSTimerHandle);
	Blackboard->SetValueAsVector("LastKnownLocation", TargetActor->GetActorLocation());
	TargetActor = nullptr;
	Blackboard->ClearValue("TargetActor");
	Blackboard->SetValueAsBool("LineOfSight", false);
	if (ChaserPawn)
		ChaserPawn->SetSprinting(false);
}
