// Fill out your copyright notice in the Description page of Project Settings.


#include "Chaser.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"

AChaser::AChaser()
	: MoveSpeed(100), SprintSpeed(600), bSprinting(false), Movement(nullptr)
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
}

void AChaser::BeginPlay()
{
	Super::BeginPlay();

	
	Movement = GetCharacterMovement();
}

void AChaser::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(AChaser, bSprinting);
}

void AChaser::SetSprinting(const bool& IsSprinting)
{
	if (!HasAuthority())
		return;
	
	if (!Movement)		
		Movement = GetCharacterMovement();
	bSprinting = IsSprinting;
	if (bSprinting)
		Movement->MaxWalkSpeed = SprintSpeed;
	else
		Movement->MaxWalkSpeed = MoveSpeed;	
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
