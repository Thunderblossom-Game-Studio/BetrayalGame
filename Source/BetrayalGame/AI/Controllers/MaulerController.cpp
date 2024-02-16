// Fill out your copyright notice in the Description page of Project Settings.


#include "MaulerController.h"

#include "NavigationPath.h"
#include "BetrayalGame/AI/Pawns/Mauler.h"
#include "NavigationSystem.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Navigation/PathFollowingComponent.h"

AMaulerController::AMaulerController()
	: MaulerState()
{
	PrimaryActorTick.bCanEverTick = true;

}

void AMaulerController::BeginPlay()
{
	Super::BeginPlay();

	Mauler = GetPawn<AMauler>();
	World = GetWorld();
	if (!Mauler)
		return;
	NavigationSystem = UNavigationSystemV1::GetCurrent(World);
	AnchorPoint = Mauler->GetActorLocation();

	// temp test
	TargetActor = Mauler->TargetTest;
}

void AMaulerController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (!World || !NavigationSystem)
		return;
	
	MaulerState = Evaluation();	
	switch (MaulerState)
	{
	case Evaluating:
		EvaluatingState();
		break;
	case Standby:
		StandbyState();
		break;
	case Acting:
		ActingState();
		break;
	default:
		break;
	}
}

TEnumAsByte<EMaulerState> AMaulerController::Evaluation()
{
	TEnumAsByte<EMaulerState> EvaluatedState = Evaluating;

	TargetActor = Mauler->TargetTest;
	const float AnchorDistance = FVector::Distance(Mauler->GetActorLocation(), AnchorPoint);
	if (TargetActor)
		EvaluatedState = Acting;
	else if (!TargetActor && AnchorDistance > 50)
		EvaluatedState = Standby;
	else
		EvaluatedState = Evaluating;
	
	return EvaluatedState;
}

void AMaulerController::EvaluatingState()
{
	GLog->Log("Evaluating");
}

void AMaulerController::StandbyState()
{	
	UNavigationPath* Path = NavigationSystem->FindPathToLocationSynchronously(World, Mauler->GetActorLocation(), AnchorPoint);
	
	MoveAlongPath(Path);
	GLog->Log("Standby");
}

void AMaulerController::ActingState()
{
	if (!TargetActor)
	{
		MaulerState = Standby;
		return;
	}

	UNavigationPath* Path = NavigationSystem->FindPathToActorSynchronously(World, Mauler->GetActorLocation(), TargetActor);
	
	MoveAlongPath(Path);
	GLog->Log("Acting");
}

void AMaulerController::MoveAlongPath(UNavigationPath* Path)
{
	const FNavPathSharedPtr PathPoints = Path->GetPath();
	if (!Path->IsValid() || PathPoints->GetPathPoints().Num() <= 1)
	{
		UE_LOG(LogTemp, Warning, TEXT("Invalid path provided!"));
		return;
	}

	const APawn* ControlledPawn = GetPawn();
	if (!ControlledPawn)
	{
		UE_LOG(LogTemp, Warning, TEXT("No controlled pawn found for AI controller!"));
		return;
	}

	MovePosition = PathPoints->GetPathPointLocation(PosIndex).Position;
	
	// Command AI pawn to move along the path
	UAIBlueprintHelperLibrary::SimpleMoveToLocation(this, MovePosition);
	if (GetMoveStatus() == EPathFollowingStatus::Type::Idle)
	{
		PosIndex++;
		if (PosIndex >= PathPoints->GetPathPoints().Num())
			PosIndex = PathPoints->GetPathPoints().Num()-1;
	}		PosIndex = PathPoints->GetPathPoints().Num()-1;
}