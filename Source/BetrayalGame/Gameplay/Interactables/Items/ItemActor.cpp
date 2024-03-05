// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemActor.h"

#include "../../Player/BaseCharacter.h"
#include "../../Player/Player Components/InventoryComponent.h"
#include "../../Player/PlayerCharacter.h"
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
	DOREPLIFETIME(AItemActor, bIsObjectiveItem);
	DOREPLIFETIME(AItemActor, bCanPickup);
}

void AItemActor::OnInteract(AActor* Interactor)
{
	Super::OnInteract(Interactor);

	if(!bCanPickup)
		return;
	
	FItem Item = *ItemData.DataTable->FindRow<FItem>(ItemData.RowName, "");
	
	APlayerCharacter* Player = Cast<APlayerCharacter>(Interactor);
	
	if(Player->InventoryComponent->IsInventoryFull())
		return;
	
	Player->InventoryComponent->Server_AddItemToInventory(Item);

	Player->OnItemPickedUp(Player->InventoryComponent->GetLastSlotAdded());

	OnPickup(Player);
	
	Destroy();
}

void AItemActor::Server_SetCanPickup_Implementation(const bool CanPickup)
{
	SetCanPickup(CanPickup);
}

// Called every frame
void AItemActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

