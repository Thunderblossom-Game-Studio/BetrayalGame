// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ItemActor.h"
#include "Components/ActorComponent.h"
#include "ObjectivesComponent.generated.h"

UENUM()
enum ERewardType
{
	RT_None UMETA(DisplayName = "None"),
	RT_Item UMETA(DisplayName = "Item"),
	RT_RandomItem UMETA(DisplayName = "Random Item"),
};

USTRUCT(BlueprintType)
struct FEventObjective : public FTableRowBase
{
	GENERATED_BODY()

	int ID;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Objective")
	FString Name;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Objective")
	FString Description;

	UPROPERTY(BlueprintReadOnly, Category = "Objective")
	bool bIsCompleted;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Objective")
	TEnumAsByte<ERewardType> RewardType;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Objective")
	AItemActor* RewardItem;
};

USTRUCT()
struct FHauntObjective : public FTableRowBase
{
	GENERATED_BODY()

	int ID;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Objective")
	FString Name;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Objective")
	FString Description;

	UPROPERTY(BlueprintReadOnly, Category = "Objective")
	bool bIsCompleted;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Objective")
	bool bIsTraitorObjective;
	
};



UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BETRAYALGAME_API UObjectivesComponent : public UActorComponent
{
	GENERATED_BODY()

private:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadOnly, Category = "Objectives")
	FEventObjective CurrentEventObjective;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadOnly, Category = "Objectives")
	FHauntObjective CurrentHauntObjective;

public:
	UObjectivesComponent();
	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION()
	void SetEventObjective(FEventObjective NewObjective);

	UFUNCTION()
	void SetHauntObjective(FHauntObjective NewObjective);

		
};
