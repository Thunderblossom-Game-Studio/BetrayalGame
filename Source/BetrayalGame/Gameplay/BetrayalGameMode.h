// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "BetrayalGameMode.generated.h"

/**
 * 
 */
UCLASS()
class BETRAYALGAME_API ABetrayalGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	void StartMatch() override;
	
};
