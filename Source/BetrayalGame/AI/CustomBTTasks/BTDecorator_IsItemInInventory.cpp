// Fill out your copyright notice in the Description page of Project Settings.


#include "BTDecorator_IsItemInInventory.h"

#include "AIController.h"
#include "BetrayalGame/Gameplay/Player/PlayerCharacter.h"

bool UBTDecorator_IsItemInInventory::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp,
                                                                uint8* NodeMemory) const
{
	const AAIController* Controller = OwnerComp.GetAIOwner();
	if (!Controller)
		return false;
	APlayerCharacter* Character = Controller->GetPawn<APlayerCharacter>();
	if (!Character)
		return false;
	UInventoryComponent* Inventory = Character->InventoryComponent;
	if (!Inventory)
		return false;
	FInventorySlot ItemSlot = GetItemInventorySlot(Inventory);
	if (ItemSlot.ID == 15)
		return false;

	if (bEquipIfFound && ItemSlot.ID != Inventory->GetSelectedSlot().ID)
	{
		switch (int SlotId = ItemSlot.ID)
		{
		case 0:
			Character->SelectSlot1();
			break;
		case 1:			
			Character->SelectSlot2();
			break;
		case 2:			
			Character->SelectSlot3();
			break;
		case 3:
			Character->SelectSlot4();
			break;
		default:
			break;
		}	
	}
	
	return true;
}

FInventorySlot UBTDecorator_IsItemInInventory::GetItemInventorySlot(UInventoryComponent* Inventory) const
{
	FInventorySlot ItemSlot;
	if (!Inventory)
		return ItemSlot;	
	const FItem* Item = ItemData.DataTable->FindRow<FItem>(ItemData.RowName, "");
	ItemSlot = Inventory->GetItemSlot(*Item);	
	return ItemSlot;
}
