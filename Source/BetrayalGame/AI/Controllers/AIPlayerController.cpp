// Fill out your copyright notice in the Description page of Project Settings.


#include "AIPlayerController.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "BetrayalGame/AI/Pawns/Monster.h"
#include "BetrayalGame/Gameplay/BetrayalGameMode.h"
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

void AAIPlayerController::EnableAIPlayer()
{
	if (GetLocalRole() == ROLE_Authority)
	{
		if (BehaviourTree)
		{
			RunBehaviorTree(BehaviourTree);						
		}
		if (PerceptionComponent)
		{
			PerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &AAIPlayerController::OnSenseTargetUpdated);
		}
	}
}

void AAIPlayerController::BeginPlay()
{
	Super::BeginPlay();

	World = GetWorld();
	if (World)
		BetrayalGameMode = World->GetAuthGameMode<ABetrayalGameMode>();	
	PlayerCharacter = GetPawn<APlayerCharacter>();
}

void AAIPlayerController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (!HasAuthority())
		return;
	if (BetrayalGameMode && Blackboard)
	{
		if (BetrayalGameMode->GetMatchStage() == Haunting)
		{
			Blackboard->SetValueAsBool("Haunt", true);
			
			if (PlayerCharacter)
				PlayerCharacter->RunStart();
		}
		else
		{
			Blackboard->SetValueAsBool("Haunt", false);
			
			if (PlayerCharacter)
				PlayerCharacter->RunEnd();
		}
	}
}

void AAIPlayerController::OnSenseTargetUpdated(AActor* UpdatedActor, FAIStimulus Stimulus)
{
	if (!UpdatedActor || !HasAuthority())
		return;	
	if (UpdatedActor->IsA(AMonster::StaticClass()))
		SeeMonster(UpdatedActor, Stimulus);
	else if (UpdatedActor->IsA(AItemActor::StaticClass()))
		SeeItem(UpdatedActor, Stimulus);
}

void AAIPlayerController::SeeMonster(AActor* UpdatedActor, FAIStimulus Stimulus)
{
	if (Stimulus.WasSuccessfullySensed())
	{
		World->GetTimerManager().ClearTimer(MonsterLOSTimerHandle);

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
		LoseDelegate.BindUFunction(this, FName("MonsterLOSRecaptureFail"), UpdatedActor);
		World->GetTimerManager().SetTimer(MonsterLOSTimerHandle, LoseDelegate, LineOfSightTimer, false);
	}
	return;
}

void AAIPlayerController::MonsterLOSRecaptureFail()
{
	World->GetTimerManager().ClearTimer(MonsterLOSTimerHandle);	
	AttackingMonster = nullptr;	
	Blackboard->ClearValue("Attacker");
	Blackboard->SetValueAsBool("LineOfSight", false);
	if (!BetrayalGameMode)
		return;	
	if (PlayerCharacter && BetrayalGameMode->GetMatchStage() != Haunting)
			PlayerCharacter->RunEnd();
}

void AAIPlayerController::SeeItem(AActor* UpdatedActor, FAIStimulus Stimulus)
{
	AItemActor* Item = Cast<AItemActor>(UpdatedActor);
	if (!Item)
		return;
	if (Stimulus.WasSuccessfullySensed())
	{
		if (!PlayerCharacter)
			return;
		//World->GetTimerManager().ClearTimer(ItemLOSTimerHandle);		
		if (!Item->GetCanPickup() || TargetItem)
			return;
		TargetItem = Item;
		Blackboard->SetValueAsObject("Item", TargetItem);
	}
	else if (TargetItem && TargetItem == Item)
	{
		FTimerDelegate LoseDelegate;
		LoseDelegate.BindUFunction(this, FName("ItemLOSRecaptureFail"), UpdatedActor);
		World->GetTimerManager().SetTimer(ItemLOSTimerHandle, LoseDelegate, LineOfSightTimer, false);
	}
	return;
}

void AAIPlayerController::ItemLOSRecaptureFail()
{
	if (!Blackboard)
		return;
	TargetItem = nullptr;
	Blackboard->ClearValue("Item");
}