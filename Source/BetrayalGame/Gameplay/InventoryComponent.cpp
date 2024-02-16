// Fill out your copyright notice in the Description page of Project Settings.


#include "../Gameplay/InventoryComponent.h"

#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Widgets/InventoryHUD.h"

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
}

// Called when the game starts
void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	InventoryBoxWidget = CreateWidget<UInventoryHUD>(GetWorld(), InventoryBoxWidgetClass);
	if(InventoryBoxWidget)
	{
		InventoryBoxWidget->AddToViewport();
	}

	for( int i = 0; i < MaxInventorySlots; i++)
	{
		UInventorySlotHUD* NewSlotWidget = CreateWidget<UInventorySlotHUD>(GetWorld(), InventorySlotWidgetClass);
		if(NewSlotWidget && InventoryBoxWidget)
		{
			SlotWidgets.Add(NewSlotWidget);
			InventoryBoxWidget->InventorySlotHolderBox->AddChild(NewSlotWidget);
		}
	}
	
	
	if(GetOwnerRole() == ROLE_Authority)
	{
		InitializeInventory();
	}
	else if (GetOwnerRole() == ROLE_SimulatedProxy)
	{
		Server_InitializeInventory();
	}
	

	
}

// Called every frame
void UInventoryComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
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

			if(slot.ID == 0)
				slot.bIsSelected = true;
			else
				slot.bIsSelected = false;
			
			FilledSlotCount++;

			SlotWidgets[slot.ID]->ItemImage->SetBrushFromTexture(slot.Item.Image);
			break;
		}
	}

	if(InventorySlots.Num() == FilledSlotCount)
	{
		bIsInventoryFull = true;
	}
	
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
		}
		else
		{
			slot.bIsSelected = false;
		}
	}
}
