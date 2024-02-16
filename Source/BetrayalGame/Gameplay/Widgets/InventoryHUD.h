// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/HorizontalBox.h"
#include "InventoryHUD.generated.h"

/**
 * 
 */
UCLASS()
class BETRAYALGAME_API UInventoryHUD : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	UHorizontalBox* InventorySlotsBox;
	
};
