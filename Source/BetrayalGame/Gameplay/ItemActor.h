// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseInteractable.h"
#include "GameFramework/Actor.h"
#include "Engine/DataTable.h"
#include "ItemActor.generated.h"

USTRUCT(BlueprintType)
struct FItem : public FTableRowBase
{
	GENERATED_BODY()

	FItem()
	{
		ItemName = FText::FromString("None");
		ItemDescription = FText::FromString("None");
		ItemIcon = nullptr;
		ItemActor = nullptr;
	}
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	FText ItemName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	FText ItemDescription;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	UTexture2D* ItemIcon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	TSubclassOf<AItemActor> ItemActor;
};


UCLASS()
class BETRAYALGAME_API AItemActor : public ABaseInteractable
{
	GENERATED_BODY()

#pragma region Base Functions
public:
	// Sets default values for this actor's properties
	AItemActor();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
private:

	
#pragma endregion

#pragma region Networking
public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
private:
#pragma endregion 
	
#pragma region Interactable Overrides
public:
	
	virtual void OnInteract(class AActor* Interactor) override;

private:
#pragma endregion
	
#pragma region Item
public:
	
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Item")
	FDataTableRowHandle ItemData;

	
private:
#pragma endregion 

};
