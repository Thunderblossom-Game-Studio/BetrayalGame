// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interactable.h"
#include "GameFramework/Actor.h"
#include "BaseInteractable.generated.h"

UCLASS()
class BETRAYALGAME_API ABaseInteractable : public AActor, public IInteractable
{
	GENERATED_BODY()

#pragma region Components
// Components
protected:
	// Needed for A.I perception system.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components, meta = (AllowPrivateAccess = "true"))
	class UAIPerceptionStimuliSourceComponent* AIStimuliSourceComponent;
	
public:
	UAIPerceptionStimuliSourceComponent* GetAIStimuliSourceComponent() const { return AIStimuliSourceComponent; }
	
#pragma endregion	
	
#pragma region Interaction
public:
	
	virtual void OnInteract(class AActor* Interactor);

	UFUNCTION(BlueprintImplementableEvent, Category = "Interaction")
	void InteractEvent(class AActor* Interactor);
	
	UFUNCTION()
	virtual void OnBeginFocus(class AActor* Interactor);

	UFUNCTION()
	virtual void OnEndFocus(class AActor* Interactor);

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Interaction")
	bool bIsInteractable = true;

private:
#pragma endregion 

public:	
	// Sets default values for this actor's properties
	ABaseInteractable();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void PostInitializeComponents() override;
	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
