// Fill out your copyright notice in the Description page of Project Settings.


#include "AIPlayerController.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "BetrayalGame/AI/Pawns/Monster.h"
#include "BetrayalGame/BetrayalGameMode.h"
#include "GameplayTagContainer.h"
#include "BetrayalGame/Gameplay/Player/PlayerCharacter.h"
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
			GLog->Log("Started behaviour tree: " + BehaviourTree->GetName());
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
}

void AAIPlayerController::SetHauntBehaviours()
{
	UBehaviorTreeComponent* BehaviourTreeComponent = Cast<UBehaviorTreeComponent>(BrainComponent);
	if (!BehaviourTreeComponent)
		return;
	
	
	if (HauntInnocentTree)
	{
		const FString InnocentTag = TEXT("DefaultPlayerInno");
		BehaviourTreeComponent->SetDynamicSubtree(FGameplayTag::RequestGameplayTag(FName(*InnocentTag)), HauntInnocentTree);
		GLog->Log("Innocent Haunt is " + HauntInnocentTree->GetName());
	}
	if (HauntTraitorTree)
	{
		const FString TraitorTag = TEXT("DefaultPlayerTraitor");
		BehaviourTreeComponent->SetDynamicSubtree(FGameplayTag::RequestGameplayTag(FName(*TraitorTag)), HauntTraitorTree);
		GLog->Log("Traitor Haunt is " + HauntTraitorTree->GetName());
	}
	
	if (Blackboard)
	{		
		Blackboard->SetValueAsBool("Haunt", true);
		GLog->Log("Blackboard Haunt is true");
	}

}

void AAIPlayerController::OnSenseTargetUpdated(AActor* UpdatedActor, FAIStimulus Stimulus)
{
	if (!UpdatedActor || !HasAuthority())
		return;	
	if (UpdatedActor->IsA(ABaseCharacter::StaticClass()))
		SeeCharacter(UpdatedActor, Stimulus);
	else if (UpdatedActor->IsA(AItemActor::StaticClass()))
		SeeItem(UpdatedActor, Stimulus);
}

void AAIPlayerController::SeeCharacter(AActor* UpdatedActor, FAIStimulus Stimulus)
{
	if (Stimulus.WasSuccessfullySensed())
	{
		World->GetTimerManager().ClearTimer(CharacterLOSTimerHandle);

		if (CharacterInView)
			return;
		
		CharacterInView = Cast<ABaseCharacter>(UpdatedActor);
		if (!CharacterInView)
			return;
		Blackboard->SetValueAsObject("CharacterInView", CharacterInView);
		if (PlayerCharacter)
			PlayerCharacter->RunStart();
		//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, "Detected Actor: " + TargetActor->GetFName().ToString());
		Blackboard->SetValueAsBool("LineOfSight", true);
	}
	else if (CharacterInView && CharacterInView == UpdatedActor)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue, "Detected Actor: " + TargetActor->GetFName().ToString());
		FTimerDelegate LoseDelegate;
		LoseDelegate.BindUFunction(this, FName("CharacterLOSRecaptureFail"), UpdatedActor);
		World->GetTimerManager().SetTimer(CharacterLOSTimerHandle, LoseDelegate, LineOfSightTimer, false);
	}
}

void AAIPlayerController::CharacterLOSRecaptureFail()
{
	World->GetTimerManager().ClearTimer(CharacterLOSTimerHandle);	
	CharacterInView = nullptr;	
	Blackboard->ClearValue("CharacterInView");
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
		if (!Item->GetCanPickup())
			return;
		Blackboard->SetValueAsObject("Item", Item);
	}
	else if (Blackboard->GetValueAsObject("Item"))
	{
		Blackboard->ClearValue("Item");
	}
}