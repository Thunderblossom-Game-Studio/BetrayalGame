// Fill out your copyright notice in the Description page of Project Settings.


#include "../Gameplay/InventoryComponent.h"

#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

// Sets default values for this component's properties
UInventoryComponent::UInventoryComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	SetIsReplicatedByDefault(true);

	for( int i = 0; i < MaxInventorySlots; i++)
	{
		FInventorySlot Slot;
		Slot.ID = i;
		Slot.bIsEmpty = true;
		Slot.bIsSelected = false;
		InventorySlots.Add(Slot);
	}
}

void UInventoryComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UInventoryComponent, InventorySlots);
}


void UInventoryComponent::Server_AddItemToInventory_Implementation(FItem Item)
{
	for (auto slot : InventorySlots)
	{
		if(slot.bIsEmpty)
		{
			slot.Item = Item;
			slot.bIsEmpty = false;
			break;
		}
		
	}

	GEngine->AddOnScreenDebugMessage(-10, 3.0f, FColor::Green, "Item: " + Item.Name.ToString() + " to " + GetOwner()->GetName() + "'s inventory");
	
}

// Called when the game starts
void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();
	
}


// Called every frame
void UInventoryComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

FInventorySlot UInventoryComponent::GetSlot(int ID)
{
	for (auto slot : InventorySlots)
		if(slot.ID == ID)
			return slot;

	return FInventorySlot();
}

FItem UInventoryComponent::GetItemInSlot(int ID)
{
	for (auto slot : InventorySlots)
		if(slot.ID == ID)
			return slot.Item;

	return FItem();
}

void UInventoryComponent::AddItemToInventory(FItem Item)
{
	
	
}

