// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_PickupItem.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"
#include "BetrayalGame/Gameplay/Player/Player Components/InventoryComponent.h"
#include "BetrayalGame/Gameplay/Player/PlayerCharacter.h"
#include "BetrayalGame/Gameplay/Interactables/Items/ItemActor.h"

UBTTask_PickupItem::UBTTask_PickupItem(FObjectInitializer const& ObjectInitializer)
	: World(nullptr), Blackboard(nullptr), Controller(nullptr),	Character(nullptr)
{
	NodeName = TEXT("Pickup Item");
	BlackboardKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UBTTask_PickupItem, BlackboardKey), AItemActor::StaticClass());
}

EBTNodeResult::Type UBTTask_PickupItem::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Controller = OwnerComp.GetAIOwner();
	Blackboard = Controller->GetBlackboardComponent();
	Character = Controller->GetPawn<APlayerCharacter>();
	if (!Blackboard || !Character)
		return EBTNodeResult::Failed;
	
	UObject* KeyValue = Blackboard->GetValue<UBlackboardKeyType_Object>(BlackboardKey.GetSelectedKeyID());
	if (AItemActor* TargetActor = Cast<AItemActor>(KeyValue))
	{
		Character->InteractableInFocus = TargetActor;
		Character->Server_Interact(Character, TargetActor);
		//GLog->Log(Character->GetName() + ": Item Picked up...");
		if (!bAutoEquip)
			return EBTNodeResult::Succeeded;
		
		const FDataTableRowHandle ItemData = TargetActor->ItemData;
		const FItem* Item = ItemData.DataTable->FindRow<FItem>(ItemData.RowName, "");
		const FInventorySlot Slot = Character->InventoryComponent->GetItemSlot(*Item);
		switch (int SlotId = Slot.ID)
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
		//GLog->Log("Item Auto Equipped..");
		return EBTNodeResult::Succeeded;
	}
	else
	{
		// if (IsValid((KeyValue)))
		// 	GLog->Log(Character->GetName() + ": " + KeyValue->GetName() + "failed to pick up...");
		// else
		// 	GLog->Log(Character->GetName() + ": Null failed to pick up...");

	}
	return EBTNodeResult::Failed;
}
