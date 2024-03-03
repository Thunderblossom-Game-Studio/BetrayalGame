// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PlayerCharacter.h"
#include "UObject/NoExportTypes.h"
#include "BaseHaunt.generated.h"

UENUM()
enum EHauntCategory
{
	Hc_Asymmetric UMETA(DisplayName = "Asymmetric"),
	Hc_HiddenAsymmetric UMETA(DisplayName = "Hidden Asymmetric"),
	Hc_FreeForAll UMETA(DisplayName = "Free For All"),
};



UCLASS(Blueprintable)
class BETRAYALGAME_API UBaseHaunt : public UObject
{
	GENERATED_BODY()

private:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	
	
protected:
	UPROPERTY(EditAnywhere, Category = "Haunt")
	FName HauntName;
	
	UPROPERTY(EditAnywhere, Category = "Haunt", meta = (MultiLine = true));
	FText HauntDescription;
	
	UPROPERTY(EditAnywhere, Category = "Haunt")
	TEnumAsByte<EHauntCategory> HauntCategory;
	
	UPROPERTY(EditAnywhere, Category = "Haunt")
	bool bHasTimer;
	
	UPROPERTY(EditDefaultsOnly, Category = "Haunt", meta = (EditCondition = "bHasTimer"))
	float HauntDuration = 0.0f;

#pragma region Traitor Properties
	UPROPERTY(VisibleAnywhere, Category = "Haunt|Traitor")
	bool bHasTraitor;
	
	UPROPERTY(EditAnywhere,
			BlueprintReadOnly, Category = "Haunt|Traitor",
			meta = (EditCondition = "HauntCategory == EHauntCategory::Hc_Asymmetric || HauntCategory == EHauntCategory::Hc_HiddenAsymmetric",
					Tooltip = "Traitors only have an objective if the haunt is Asymmetric or Hidden Asymmetric"))
	FDataTableRowHandle TraitorObjective;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Haunt|Traitor",
			meta = (EditCondition = "HauntCategory == EHauntCategory::Hc_Asymmetric || HauntCategory == EHauntCategory::Hc_HiddenAsymmetric",
					Tooltip = "Traitors only have a list of monsters if the haunt is Asymmetric or Hidden Asymmetric"))
	TArray<AMonster*> TraitorMonsters;
#pragma endregion

#pragma region Survivor Properties
	UPROPERTY(EditAnywhere,
			BlueprintReadOnly, Category = "Haunt|Survivors",
			meta=(Tooltip = "Survivors will always have an objective"))
	FDataTableRowHandle SurvivorObjective;
#pragma endregion 
	
public:
	// TODO: Add Constructor
	// TODO: Add cutscene references(start haunt, end haunt, traitor picked)
	
	UFUNCTION(Blueprintable, Category = "Haunt")
	void StartHaunt();

	UFUNCTION(BlueprintImplementableEvent, Category = "Haunt")
	void OnHauntStart();

	UFUNCTION(BlueprintNativeEvent, Category = "Haunt")
	void OnHauntEnd();

	UFUNCTION(BlueprintNativeEvent, Category = "Haunt")
	void OnTraitorPicked(APlayerCharacter* TraitorPlayer);

	
};
