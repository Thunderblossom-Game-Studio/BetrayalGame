// Fill out your copyright notice in the Description page of Project Settings.


#include "Chaser.h"

#include "GameFramework/CharacterMovementComponent.h"

AChaser::AChaser()
	: MoveSpeed(400), SprintSpeed(600), bSprinting(false)
{
}

void AChaser::SetSprinting(const bool& IsSprinting)
{
	bSprinting = IsSprinting;
	if (bSprinting)
		GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
	else
		GetCharacterMovement()->MaxWalkSpeed = MoveSpeed;
}

void AChaser::Teleport(AActor* Target)
{
	if (!Target)
		return;
	
	OnTeleport(Target);

	Target->SetActorLocation(TeleportDestination);
}
