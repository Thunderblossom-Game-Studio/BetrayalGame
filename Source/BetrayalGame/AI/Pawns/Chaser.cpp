// Fill out your copyright notice in the Description page of Project Settings.


#include "Chaser.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"

AChaser::AChaser()
	: Movement(nullptr)
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
}

void AChaser::BeginPlay()
{
	Super::BeginPlay();

	
	Movement = GetCharacterMovement();
}

void AChaser::SetChasing(const bool& IsChasing)
{
	if (!HasAuthority())
		return;
	
	if (!Movement)		
		Movement = GetCharacterMovement();
	bIsRunning = IsChasing;
	if (bIsRunning)
		Movement->MaxWalkSpeed = RunSpeed;	
	else
		Movement->MaxWalkSpeed = WalkSpeed;
}

void AChaser::Attack(AActor* Target)
{	
	if (!HasAuthority() || !Target)
		return;
	
	Super::Attack(Target);	
	Teleport(Target);
}

void AChaser::Teleport(AActor* Target)
{
	if (!HasAuthority())
		return;
	if (!Target)
		return;
	
	OnTeleport(Target);

	Target->SetActorLocation(TeleportDestination);
}
