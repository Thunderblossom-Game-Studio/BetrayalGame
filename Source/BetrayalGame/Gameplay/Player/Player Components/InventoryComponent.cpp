// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryComponent.h"

#include "../PlayerCharacter.h"
#include "Net/UnrealNetwork.h"

// Sets default values for this component's properties
UInventoryComponent::UInventoryComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	SetIsReplicatedByDefault(true);
}

void UInventoryComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UInventoryComponent, InventorySlots);
	DOREPLIFETIME(UInventoryComponent, FilledSlotCount);
	DOREPLIFETIME(UInventoryComponent, bIsInventoryFull);
	DOREPLIFETIME(UInventoryComponent, bIsInventoryInitialized);
	DOREPLIFETIME(UInventoryComponent, SelectedSlot);
}

// Called when the game starts
void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();
	
	if(GetOwnerRole() == ROLE_Authority)
		InitializeInventory();
	else if (GetOwnerRole() == ROLE_SimulatedProxy)
		Server_InitializeInventory();
}

void UInventoryComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UInventoryComponent::Server_AddItemToInventory_Implementation(FItem Item)
{
	AddItemToInventory(Item);
}

void UInventoryComponent::AddItemToInventory(FItem Item)
{
	for (auto& slot : InventorySlots)
	{
		if(slot.bIsEmpty)
		{
			slot.Item = Item;
			slot.bIsEmpty = false;

			SelectSlot(slot.ID);
			
			LastSlotAdded = slot;
			
			FilledSlotCount++;
			break;
		}
	}

	if(InventorySlots.Num() == FilledSlotCount)
	{
		bIsInventoryFull = true;
	}
}

void UInventoryComponent::RemoveItemFromInventory(int ID)
{
	for (auto& slot : InventorySlots)
	{
		if(slot.ID == ID)
		{
			slot.Item = FItem();
			slot.bIsEmpty = true;
			FilledSlotCount--;
			break;
		}
	}
}

void UInventoryComponent::Server_RemoveItemFromInventory_Implementation(int ID)
{
	RemoveItemFromInventory(ID);
}

void UInventoryComponent::InitializeInventory()
{
	for ( int i = 0; i < MaxInventorySlots; i++)
	{
		FInventorySlot Slot;
		Slot.ID = i;
		Slot.bIsEmpty = true;
		InventorySlots.Add(Slot);
	}
	
	bIsInventoryInitialized = true;
}

void UInventoryComponent::Server_InitializeInventory_Implementation()
{
	InitializeInventory();
}

FInventorySlot UInventoryComponent::GetSlot(int ID)
{
	for (auto slot : InventorySlots)
		if(slot.ID == ID)
			return slot;

	return FInventorySlot();
}

FInventorySlot UInventoryComponent::GetItemSlot(FItem Item)
{
	for (auto slot : InventorySlots)
		if(slot.Item.Name.ToString() == Item.Name.ToString())
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

void UInventoryComponent::SelectSlot(int ID)
{
	for (auto& slot : InventorySlots)
	{
		if(slot.ID == ID)
		{
			slot.bIsSelected = true;
			SelectedSlot = slot;
		}
		else
		{
			slot.bIsSelected = false;
		}
	}
}
