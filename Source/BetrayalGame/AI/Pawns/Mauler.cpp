// Fill out your copyright notice in the Description page of Project Settings.


#include "Mauler.h"
#include "../MaulerAnimInstance.h"
#include "BetrayalGame/BetrayalPlayerState.h"
#include "BetrayalGame/Gameplay/Player/PlayerCharacter.h"
#include "Components/SphereComponent.h"
#include "Net/UnrealNetwork.h"

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

void AMauler::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AMauler, bAttacking);
}

void AMauler::BeginPlay()
{
	Super::BeginPlay();
	if (const USkeletalMeshComponent* MeshComponent = GetMesh())
		MaulerAnim = Cast<UMaulerAnimInstance>(MeshComponent->GetAnimInstance());
	
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
	APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(TargetCharacter);
	if (!PlayerCharacter)
	{
		TargetCharacter = FindClosestCharacter();		
		return;
	}
	FHitResult HitResult;
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(this); 
	if (!GetWorld()->LineTraceSingleByChannel(HitResult, GetActorLocation(), PlayerCharacter->GetActorLocation(), ECollisionChannel::ECC_Visibility, CollisionParams))
		return;
	if (ValidTargets.Contains(PlayerCharacter))
	{
		TargetCharacter = nullptr;
		TargetCharacter = FindClosestCharacter();
	}
}

void AMauler::Attack(AActor* Target)
{	
	if (!Target)
		return;
	Super::Attack(Target);
	bAttacking = true;
	if (!Target->IsA<ABaseCharacter>())
		return;
	if (ABaseCharacter* Character = Cast<ABaseCharacter>(Target))
	{
		const int DamageToTake = Character->GetCurrentHealth() - 1;
		Character->Server_TakeDamage(DamageToTake);
		
		//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Purple, "Health: " + FString::SanitizeFloat(Character->GetCurrentHealth()));
	}
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
		if (Distance >= SmallestDistance)
			continue;
		ClosestCharacter = Character;
		SmallestDistance = Distance;
	}
	
	return ClosestCharacter;
}

void AMauler::OverlapEnter(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
                           int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor)
		return;
	if (!OtherActor->IsA(APlayerCharacter::StaticClass()))
		return;
	APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(OtherActor);
	if (!PlayerCharacter || ValidTargets.Contains(PlayerCharacter))
		return;			
	if (ABetrayalPlayerState* BetrayalPlayerState = PlayerCharacter->GetPlayerState<ABetrayalPlayerState>())
	{
		if (BetrayalPlayerState->IsTraitor())
			return;
	}
	
	ValidTargets.Add(PlayerCharacter);
	TargetCharacter = FindClosestCharacter();
}

void AMauler::OverlapExit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex)
{
	if (!OtherActor)
		return;	
	if (!OtherActor->IsA(APlayerCharacter::StaticClass()))
		return;
	APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(OtherActor);
	if (!ValidTargets.Contains(PlayerCharacter))
		return;
	ValidTargets.Remove(PlayerCharacter);			
	TargetCharacter = FindClosestCharacter();
}
