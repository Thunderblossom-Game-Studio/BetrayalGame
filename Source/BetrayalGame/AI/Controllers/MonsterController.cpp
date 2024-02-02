// Fill out your copyright notice in the Description page of Project Settings.


#include "MonsterController.h"

AMonsterController::AMonsterController()
: BehaviourTreeComponent(nullptr), SightConfig(nullptr), RoomTraversalType(), TargetPawn(nullptr)
{
}

void AMonsterController::BeginPlay()
{
	Super::BeginPlay();

}
