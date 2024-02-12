// Fill out your copyright notice in the Description page of Project Settings.


#include "../Gameplay/ItemActor.h"

#include "BaseCharacter.h"
#include "PlayerCharacter.h"
#include "Net/UnrealNetwork.h"

// Sets default values
AItemActor::AItemActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
}

// Called when the game starts or when spawned
void AItemActor::BeginPlay()
{
	Super::BeginPlay();
	
}

void AItemActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AItemActor, ItemData);
}

void AItemActor::OnInteract(AActor* Interactor)
{
	Super::OnInteract(Interactor);

	FItem Item = *ItemData.DataTable->FindRow<FItem>(ItemData.RowName, "Paper");
	//FItem Item* = ItemData.DataTable->FindRow<FItem>(ItemData.RowName, "Paper");

	// Cast interactor to player
	APlayerCharacter* Player = Cast<APlayerCharacter>(Interactor);

	Player->Server_AddItemToInventory(Item, Player);

	GEngine->AddOnScreenDebugMessage(-11, 3.0f, FColor::Red, "Player " + Interactor->GetName() + " picked up " + Player->ActorItem.ItemName.ToString());
	
	//GEngine->AddOnScreenDebugMessage(-10, 3.0f, FColor::Red, Interactor->GetName() + " Interacted with " + Item.ItemDescription.ToString());
	
	Destroy();
	//GEngine->AddOnScreenDebugMessage(-10, 3.0f, FColor::Red, Interactor. + " Interacted with " + ItemData.DataTable.GetName());
}

// Called every frame
void AItemActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

