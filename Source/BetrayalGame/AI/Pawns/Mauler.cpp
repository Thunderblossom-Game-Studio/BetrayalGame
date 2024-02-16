// Fill out your copyright notice in the Description page of Project Settings.


#include "Mauler.h"

#include "BetrayalGame/Gameplay/PlayerCharacter.h"
#include "Components/SphereComponent.h"

AMauler::AMauler()
{
	PrimaryActorTick.bCanEverTick = true;
	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere Component"));
	if (SphereComponent)
	{
		SphereComponent->SetupAttachment(RootComponent);
		SphereComponent->SetSphereRadius(750);
	}
}

void AMauler::BeginPlay()
{
	Super::BeginPlay();
	if (SphereComponent)
	{
		SphereComponent->OnComponentBeginOverlap.AddDynamic(this, &AMauler::OverlapEnter);
		SphereComponent->OnComponentEndOverlap.AddDynamic(this, &AMauler::OverlapExit);
		SphereComponent->SetWorldLocation(GetActorLocation());
	}
	AnchorPoint = GetActorLocation();
}

void AMauler::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (ValidTargets.Num() == 0)
		return;
	APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(TargetTest);
	if (!PlayerCharacter)
	{
		TargetTest = FindClosestCharacter();		
		return;
	}
	FHitResult HitResult;
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(this); 
	if (!GetWorld()->LineTraceSingleByChannel(HitResult, GetActorLocation(), PlayerCharacter->GetActorLocation(), ECollisionChannel::ECC_Visibility, CollisionParams))
		return;
	if (ValidTargets.Contains(PlayerCharacter))
	{
		TargetTest = nullptr;
		TargetTest = FindClosestCharacter();
	}
	GLog->Log("Sasdsakdlasdna");
}

void AMauler::Attack(AActor* Target)
{
	if (!Target)
		return;
	Super::Attack(Target);
		
}

APlayerCharacter* AMauler::FindClosestCharacter()
{
	if (ValidTargets.Num() == 0)
		return nullptr;
	else if (ValidTargets.Num() == 1)
	{
		FHitResult HitResult;
		FCollisionQueryParams CollisionParams;
		CollisionParams.AddIgnoredActor(this); 
		if (GetWorld()->LineTraceSingleByChannel(HitResult, GetActorLocation(), ValidTargets[0]->GetActorLocation(), ECollisionChannel::ECC_Visibility, CollisionParams))
			return nullptr;
		return ValidTargets[0];
	}
	
	APlayerCharacter* ClosestCharacter = ValidTargets[0];
	float SmallestDistance = FVector::Distance(ValidTargets[0]->GetActorLocation(), GetActorLocation());
	for (APlayerCharacter* Character : ValidTargets)
	{
		if (Character == nullptr || ClosestCharacter == Character)
			continue;
		FHitResult HitResult;
		FCollisionQueryParams CollisionParams;
		CollisionParams.AddIgnoredActor(this); 
		if (GetWorld()->LineTraceSingleByChannel(HitResult, GetActorLocation(), Character->GetActorLocation(), ECollisionChannel::ECC_Visibility, CollisionParams))
			continue;
		const float Distance = FVector::Distance(Character->GetActorLocation(), GetActorLocation());
		if (Distance < SmallestDistance)
		{
			ClosestCharacter = Character;
			SmallestDistance = Distance;
		}
	}
	
	return ClosestCharacter;
}

void AMauler::OverlapEnter(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
                           int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor)
		return;
	if (OtherActor->IsA(APlayerCharacter::StaticClass()))
	{
		APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(OtherActor);
		if (PlayerCharacter && !ValidTargets.Contains(PlayerCharacter))
		{
			//FHitResult HitResult;
			FCollisionQueryParams CollisionParams;
			CollisionParams.AddIgnoredActor(this);
			ValidTargets.Add(PlayerCharacter);
			TargetTest = FindClosestCharacter();
			// if (!GetWorld()->LineTraceSingleByChannel(HitResult, GetActorLocation(), PlayerCharacter->GetActorLocation(), ECollisionChannel::ECC_Visibility, CollisionParams))
			// {
			//
			// }
		}
	}
}

void AMauler::OverlapExit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex)
{
	if (!OtherActor)
		return;	
	if (OtherActor->IsA(APlayerCharacter::StaticClass()))
	{
		APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(OtherActor);
		if (ValidTargets.Contains(PlayerCharacter))
		{
			ValidTargets.Remove(PlayerCharacter);			
			TargetTest = FindClosestCharacter();
		}
	}
}
