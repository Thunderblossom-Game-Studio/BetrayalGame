// Fill out your copyright notice in the Description page of Project Settings.


#include "ChaserAnimInstance.h"
#include "Pawns/Chaser.h"
#include "Engine/Engine.h"

UChaserAnimInstance::UChaserAnimInstance()
	: MoveSpeed(0.0f), bRunning(false), Chaser(nullptr)
{
}

void UChaserAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	Chaser = Cast<AChaser>(TryGetPawnOwner());
}

void UChaserAnimInstance::CustomAnimUpdate()
{	
	if (!Chaser)
		Chaser = Cast<AChaser>(TryGetPawnOwner());	
	if (Chaser)
	{
		bRunning = Chaser->bIsRunning;
		FVector Speed = Chaser->GetVelocity();
		Speed.Z = 0.0f;
		MoveSpeed = Speed.Size();
		
		//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, "MoveSpeed " + FString::SanitizeFloat(MoveSpeed));		
		//const FString Result = (bChasing ? "true" : "false");
		//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue, "Sprinting: " + Result);
	}
}