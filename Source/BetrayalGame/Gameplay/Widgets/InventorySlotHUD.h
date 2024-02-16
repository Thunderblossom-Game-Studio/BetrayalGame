// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "InventorySlotHUD.generated.h"

/**
 * 
 */
UCLASS()
class BETRAYALGAME_API UInventorySlotHUD : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slot")
	UImage* SelectedSlotImage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slot")
	UImage* ItemImage;
	
};
