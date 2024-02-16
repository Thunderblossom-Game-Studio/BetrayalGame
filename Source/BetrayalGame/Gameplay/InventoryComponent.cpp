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
	DOREPLIFETIME(UInventoryComponent, bIsInventoryInitialized);
}

// Called when the game starts
void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(GetOwner());
	if(!PlayerCharacter)
		return;

	if(!PlayerCharacter->Controller)
		return;
	
	if(PlayerCharacter->Controller->IsLocalController())
	{
		if(PlayerCharacter->HasAuthority())
			InitializeInventoryHUD();
		else
			InitializeInventoryHUD();
	}
	
	if(GetOwnerRole() == ROLE_Authority  )
	{
		InitializeInventory();
		
		//InitializeInventoryHUD();
	}
	else if (GetOwnerRole() == ROLE_SimulatedProxy)
	{
		Server_InitializeInventory();
	}

	if (GetOwnerRole() == ROLE_AutonomousProxy)
	{
		 //InitializeInventoryHUD();
	}
	
}

void UInventoryComponent::OnRep_InventorySlots()
{
	// if(!bIsInventoryInitialized)
	// 	return;
	
	// for (auto slot : InventorySlots)
	// {
	// 	if(slot.bIsEmpty)
	// 	{
	// 		SlotWidgets[slot.ID]->ItemImage->SetVisibility(ESlateVisibility::Hidden);
	// 	}
	// 	else
	// 	{
	// 		SlotWidgets[slot.ID]->ItemImage->SetBrushFromTexture(slot.Item.Image);
	// 		SlotWidgets[slot.ID]->ItemImage->SetVisibility(ESlateVisibility::Visible);
	// 	}
	// }
}

void UInventoryComponent::OnRep_InventoryInitialized()
{
	
	// Cast to APlayerController
	//InitializeInventoryHUD();
	
    	
	// for( int i = 0; i < MaxInventorySlots; i++)
	// {
	// 	UInventorySlotHUD* NewSlotWidget = CreateWidget<UInventorySlotHUD>(GetWorld(), InventorySlotWidget);
	// 	if(NewSlotWidget && InventoryHUD)
	// 	{
	// 		SlotWidgets.Add(NewSlotWidget);
	// 		InventoryHUD->InventorySlotHolderBox->AddChild(NewSlotWidget);
	// 	}
	// }		
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
	
	bIsInventoryInitialized = true;

}

void UInventoryComponent::InitializeInventoryHUD()
{
	if(!bIsInventoryInitialized)
		return;
	
	
	InventoryHUD = CreateWidget<UInventoryHUD>(GetWorld(), InventoryHUDWidget);
	if(InventoryHUD)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Inventory HUD Created"));
		InventoryHUD->AddToViewport();
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
