// Fill out your copyright notice in the Description page of Project Settings.


#include "Monster.h"

AMonster::AMonster()
	: AttackDamage(1), AttackDamageOperation(), AttackSpeed(1)
{
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;
}

void AMonster::Attack(AActor* Target)
{
	if (!Target)
		return;
	OnAttack(Target);
}
