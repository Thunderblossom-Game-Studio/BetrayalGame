// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ItemActor.h"
#include "Components/ActorComponent.h"
#include "InventoryComponent.generated.h"


USTRUCT()
struct FInventorySlot
{
	GENERATED_BODY()
	
	FItem Item;
	bool bIsEmpty;
	bool bIsSelected;
	int Quantity;
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
	
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Inventory", meta = (AllowPrivateAccess = "true"))
	TArray<FItem> Inventory;

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	TArray<FItem> GetInventory() const { return Inventory; }
	
	UFUNCTION(Server, Reliable)
	void Server_AddItemToInventory(FItem Item);
	
	
	UPROPERTY(EditAnywhere, Category = "Inventory")
	int InventorySize;
	
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void SetInventorySize(int Size) { InventorySize = Size; }

	UPROPERTY()
	bool bIsInventoryFull;

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool IsInventoryFull() const { return bIsInventoryFull; }
		
};
