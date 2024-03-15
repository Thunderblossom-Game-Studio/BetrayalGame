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
	if (!CheckInventoryForItem(Character->InventoryComponent))
		return false;	
	return true;
}

bool UBTDecorator_IsItemInInventory::CheckInventoryForItem(UInventoryComponent* Inventory) const
{
	if (!Inventory)
		return false;
	
	const FItem* Item = ItemData.DataTable->FindRow<FItem>(ItemData.RowName, "");
	if (Inventory->GetItemSlot(*Item).ID == 15)
		return false;
	
	return true;
}
