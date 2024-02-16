// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widgets\InventoryHUD.h"
#include "ItemActor.h"
#include "PlayerCharacter.h"
#include "Components/ActorComponent.h"
#include "Components/WidgetComponent.h"
#include "InventoryComponent.generated.h"


USTRUCT()
struct FInventorySlot
{
	GENERATED_BODY()

	// If the slot is empty, the item is null and the ID is 15
	FInventorySlot()
	{
		ID = 15;
		bIsEmpty = true;
		bIsSelected = true;
	}
	
	UPROPERTY(EditAnywhere, Category = "Slot")
	FItem Item;
	
	UPROPERTY(EditAnywhere, Category = "Slot")
	int ID;
	
	UPROPERTY(EditAnywhere, Category = "Slot")
	bool bIsEmpty;
	
	UPROPERTY(EditAnywhere, Category = "Slot")
	bool bIsSelected;
	
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BETRAYALGAME_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

private:	
	// Sets default values for this component's properties
	UInventoryComponent();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;


	
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY(Replicated, VisibleAnywhere, Category = "Inventory")
	TArray<FInventorySlot> InventorySlots;

	UPROPERTY(EditAnywhere, Category = "Inventory")
	int MaxInventorySlots;
	
	UPROPERTY(Replicated, VisibleAnywhere, Category = "Inventory")
	bool bIsInventoryFull;

	UPROPERTY(Replicated, VisibleAnywhere, Category = "Inventory")
	int FilledSlotCount;
	
	UPROPERTY(EditAnywhere, Category = "Inventory|Widgets")
	TSubclassOf<UInventoryHUD> InventoryBoxWidgetClass;
	UPROPERTY(VisibleAnywhere , Category = "Inventory|Widgets")
	UInventoryHUD* InventoryBoxWidget;
	
	UPROPERTY(EditAnywhere, Category = "Inventory|Widgets")
	TSubclassOf<UUserWidget> InventorySlotWidgetClass;

	UPROPERTY(VisibleAnywhere, Category = "Inventory|Widgets")
	TArray<UUserWidget*> SlotWidgets;
	
public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	UFUNCTION()
	FInventorySlot GetSlot(int ID);
	UFUNCTION()
	FItem GetItemInSlot(int ID);

	UFUNCTION()
	void SelectSlot(int ID);
	
	UFUNCTION(Server, Reliable)
	void Server_AddItemToInventory(FItem Item);
	UFUNCTION()
	void AddItemToInventory(FItem Item);

	UFUNCTION(Server, Reliable)
	void Server_InitializeInventory();
	UFUNCTION()
	void InitializeInventory();
	
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool IsInventoryFull() const { return bIsInventoryFull; }
		
};
