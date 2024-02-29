// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Widget_LobbyMenu.generated.h"

/**
 * 
 */
UCLASS()
class BETRAYALGAME_API UWidget_LobbyMenu : public UUserWidget
{
	GENERATED_BODY()

	virtual void NativeConstruct() override;
};
