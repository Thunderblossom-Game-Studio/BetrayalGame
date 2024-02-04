// Fill out your copyright notice in the Description page of Project Settings.


#include "ChaserAnimInstance.h"
#include "Controllers/MonsterController.h"
#include "Pawns/Monster.h"

UChaserAnimInstance::UChaserAnimInstance()
	: MoveSpeed(0.0f), bChasing(false), Pawn(nullptr)
{
}

void UChaserAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	Pawn = Cast<AMonster>(TryGetPawnOwner());
	if (Pawn)
		Controller = Pawn->GetController<AMonsterController>();
}

void UChaserAnimInstance::CustomAnimUpdate()
{
	if (!Pawn)
		Pawn = Cast<AMonster>(TryGetPawnOwner());	
	if (Pawn && !Controller)
		Controller = Pawn->GetController<AMonsterController>();
	if (Pawn && Controller)
	{		
		FVector Speed = Pawn->GetVelocity();
		Speed.Z = 0.0f;
		MoveSpeed = Speed.Size();
		bChasing = (Controller->GetTargetActor() != nullptr);
	}
}