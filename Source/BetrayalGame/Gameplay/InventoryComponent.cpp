// Fill out your copyright notice in the Description page of Project Settings.


#include "../Gameplay/InventoryComponent.h"

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

	DOREPLIFETIME(UInventoryComponent, Inventory);
}


void UInventoryComponent::Server_AddItemToInventory_Implementation(FItem Item)
{
	Inventory.Add(Item);

	GEngine->AddOnScreenDebugMessage(-10, 3.0f, FColor::Green, "Item: " + Item.Name.ToString() + " to " + GetOwner()->GetName() + "'s inventory");
	
	if(Inventory.Num() >= InventorySize)
		bIsInventoryFull = true;
	
}

// Called when the game starts
void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UInventoryComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

