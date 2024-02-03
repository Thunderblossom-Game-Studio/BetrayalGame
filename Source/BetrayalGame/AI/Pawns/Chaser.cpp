// Fill out your copyright notice in the Description page of Project Settings.


#include "Chaser.h"

AChaser::AChaser()
{
}

void AChaser::Teleport(AActor* Target)
{
	if (!Target)
		return;
	
	OnTeleport(Target);

	Target->SetActorLocation(TeleportDestination);
}