// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "BetrayalPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class BETRAYALGAME_API ABetrayalPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	void BeginPlay() override;
	
};
