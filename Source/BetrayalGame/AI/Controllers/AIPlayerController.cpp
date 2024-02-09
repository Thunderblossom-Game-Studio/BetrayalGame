// Fill out your copyright notice in the Description page of Project Settings.


#include "AIPlayerController.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "BetrayalGame/AI/Pawns/Monster.h"
#include "BetrayalGame/Gameplay/PlayerCharacter.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Damage.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "Perception/AISenseConfig_Sight.h"

AAIPlayerController::AAIPlayerController()
	: BehaviourTree(nullptr), SightConfig(nullptr), HearingConfig(nullptr), DamageConfig(nullptr), World(nullptr)
{
	PrimaryActorTick.bCanEverTick = true;
	
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
	
	HearingConfig = CreateDefaultSubobject<UAISenseConfig_Hearing>(TEXT("Hearing Config"));
	if (HearingConfig)
	{
		HearingConfig->HearingRange = 500.0f;
		HearingConfig->DetectionByAffiliation.bDetectEnemies = true;
		HearingConfig->DetectionByAffiliation.bDetectNeutrals = true;
		HearingConfig->DetectionByAffiliation.bDetectFriendlies = true;
		PerceptionComponent->ConfigureSense(*HearingConfig);
	}
	
	DamageConfig = CreateDefaultSubobject<UAISenseConfig_Damage>(TEXT("Damage Config"));
	if (DamageConfig)
	{
		PerceptionComponent->ConfigureSense(*DamageConfig);
	}
}

void AAIPlayerController::BeginPlay()
{
	Super::BeginPlay();

	World = GetWorld();
	PlayerCharacter = GetPawn<APlayerCharacter>();
	
	if (GetLocalRole() == ROLE_Authority)
	{
		if (PerceptionComponent)
		{
			PerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &AAIPlayerController::OnSenseTargetUpdated);
		}
		if (BehaviourTree)
		{
			RunBehaviorTree(BehaviourTree);			
		}
	}
}

void AAIPlayerController::OnSenseTargetUpdated(AActor* UpdatedActor, FAIStimulus Stimulus)
{
	if (!UpdatedActor || !HasAuthority())
		return;	
	if (!UpdatedActor->IsA(AMonster::StaticClass()))
		return;	
	if (Stimulus.WasSuccessfullySensed())
	{
		World->GetTimerManager().ClearTimer(LOSTimerHandle);

		if (AttackingMonster)
			return;
		
		AttackingMonster = Cast<AMonster>(UpdatedActor);
		if (!AttackingMonster)
			return;
		Blackboard->SetValueAsObject("Attacker", AttackingMonster);
		if (PlayerCharacter)
			PlayerCharacter->RunStart();
		//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, "Detected Actor: " + TargetActor->GetFName().ToString());
		Blackboard->SetValueAsBool("LineOfSight", true);
	}
	else if (AttackingMonster && AttackingMonster == UpdatedActor)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue, "Detected Actor: " + TargetActor->GetFName().ToString());
		FTimerDelegate LoseDelegate;
		LoseDelegate.BindUFunction(this, FName("LOSRecaptureFail"), UpdatedActor);
		World->GetTimerManager().SetTimer(LOSTimerHandle, LoseDelegate, LineOfSightTimer, false);
	}
}

void AAIPlayerController::LOSRecaptureFail()
{
	World->GetTimerManager().ClearTimer(LOSTimerHandle);	
	AttackingMonster = nullptr;	
	Blackboard->ClearValue("Attacker");
	Blackboard->SetValueAsBool("LineOfSight", false);
	if (PlayerCharacter)
			PlayerCharacter->RunEnd();
}