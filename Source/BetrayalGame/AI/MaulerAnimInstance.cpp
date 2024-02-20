// Fill out your copyright notice in the Description page of Project Settings.


#include "MaulerAnimInstance.h"

#include "Pawns/Mauler.h"

UMaulerAnimInstance::UMaulerAnimInstance()
	: MoveSpeed(0.0f), bSprinting(false), bAttack(false), Mauler(nullptr)
{
}

void UMaulerAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	
	Mauler = Cast<AMauler>(TryGetPawnOwner());
}

void UMaulerAnimInstance::CustomAnimUpdate()
{
	if (!Mauler)		
		Mauler = Cast<AMauler>(TryGetPawnOwner());
	if (Mauler)
	{
		bSprinting = Mauler->bIsRunning;
		bAttack = Mauler->IsAttacking();
		FVector Speed = Mauler->GetVelocity();
		Speed.Z = 0.0f;
		MoveSpeed = Speed.Size();		
	}
}

void UMaulerAnimInstance::ResetAttack(const FAnimUpdateContext& Context, const FAnimNodeReference& Node)
{
	if (!Mauler)
		return;
	Mauler->ResetAttack();
}
