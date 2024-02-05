// Fill out your copyright notice in the Description page of Project Settings.


#include "ChaserAnimInstance.h"
#include "Controllers/MonsterController.h"
#include "Pawns/Chaser.h"
#include "Engine/Engine.h"

UChaserAnimInstance::UChaserAnimInstance()
	: MoveSpeed(0.0f), bChasing(false), Pawn(nullptr)
{
}

void UChaserAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	Pawn = Cast<AChaser>(TryGetPawnOwner());
}

void UChaserAnimInstance::CustomAnimUpdate()
{	
	if (!Pawn)
		Pawn = Cast<AChaser>(TryGetPawnOwner());	
	if (Pawn)
	{
		bChasing = Pawn->IsSprinting();
		FVector Speed = Pawn->GetVelocity();
		Speed.Z = 0.0f;
		MoveSpeed = Speed.Size();
		
		//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, "MoveSpeed " + FString::SanitizeFloat(MoveSpeed));		
		//const FString Result = (bChasing ? "true" : "false");
		//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue, "Sprinting: " + Result);
	}
}