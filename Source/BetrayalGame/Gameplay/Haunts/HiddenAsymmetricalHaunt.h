// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseHaunt.h"
#include "HiddenAsymmetricalHaunt.generated.h"

/**
 * 
 */
UCLASS()
class BETRAYALGAME_API AHiddenAsymmetricalHaunt : public ABaseHaunt
{
	GENERATED_BODY()

private:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
protected:

	UPROPERTY(EditAnywhere, Category = "Haunt|Hidden Asymmetric")
	bool bAddInteractableOnBeginPlay = false;
	
	UPROPERTY(EditAnywhere, Category = "Haunt|Hidden Asymmetric", meta = (EditCondition = "!bAddInteractableOnBeginPlay"))
	TSubclassOf<ABaseInteractable> ObjectiveInteractableClass;

	UPROPERTY(EditAnywhere, Category = "Haunt|Hidden Asymmetric")
	ABaseInteractable* ObjectiveInteractable;

	
	
public:
	AHiddenAsymmetricalHaunt();

	virtual void BeginPlay() override;


	
};
