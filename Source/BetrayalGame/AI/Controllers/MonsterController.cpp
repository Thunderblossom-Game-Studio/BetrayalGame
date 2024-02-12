// Fill out your copyright notice in the Description page of Project Settings.


#include "MonsterController.h"

AMonsterController::AMonsterController()
	: BehaviourTree(nullptr), SightConfig(nullptr), HearingConfig(nullptr), RoomTraversalType(), TargetActor(nullptr)
{
}

