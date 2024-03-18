// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "BetrayalGame/Gameplay/Interactables/Items/ItemActor.h"
#include "BetrayalGame/Gameplay/Player/Player Components/InventoryComponent.h"
#include "BTDecorator_IsItemInInventory.generated.h"

/**
 * 
 */
UCLASS()
class BETRAYALGAME_API UBTDecorator_IsItemInInventory : public UBTDecorator
{
	GENERATED_BODY()

public:
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;

protected:	
	UPROPERTY(EditAnywhere)
	FDataTableRowHandle ItemData;
	UPROPERTY(EditAnywhere)
	bool bEquipIfFound = false;
	UPROPERTY(EditAnywhere)
	FString ItemName;

	// Returns the inventory slot
	FInventorySlot GetItemInventorySlot(class UInventoryComponent* Inventory) const;
	
};