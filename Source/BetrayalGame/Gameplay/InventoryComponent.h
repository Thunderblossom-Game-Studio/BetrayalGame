// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ItemActor.h"
#include "PlayerCharacter.h"
#include "Components/ActorComponent.h"
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


	
public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	UPROPERTY(Replicated, VisibleAnywhere, Category = "Inventory", meta = (AllowPrivateAccess = "true"))
	TArray<FInventorySlot> InventorySlots;

	UFUNCTION()
	FInventorySlot GetSlot(int ID);

	UFUNCTION()
	FItem GetItemInSlot(int ID);
	
	UFUNCTION(Server, Reliable)
	void Server_AddItemToInventory(FItem Item);

	UFUNCTION()
	void AddItemToInventory(FItem Item);
	
	UPROPERTY(EditAnywhere, Category = "Inventory")
	int MaxInventorySlots;
	
	UPROPERTY()
	bool bIsInventoryFull;

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool IsInventoryFull() const { return bIsInventoryFull; }
		
};
