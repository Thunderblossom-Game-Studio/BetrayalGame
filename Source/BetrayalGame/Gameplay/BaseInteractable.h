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

public:
	
	UPROPERTY(Replicated)
	class ABaseCharacter* InteractableOwner;
private:
	
#pragma region Interaction
public:
	virtual void OnInteract(class ABaseCharacter* Interactor);

	UFUNCTION(Server,Reliable)
	void ServerOnInteract(class ABaseCharacter* Interactor);

	UFUNCTION(NetMulticast,Reliable)
	void NetMulticastOnInteract(class ABaseCharacter* Interactor);

	UFUNCTION(Server,Reliable)
	void Server_Destroy(class AActor* Who);
		
	UFUNCTION()
	virtual void OnBeginFocus(class ABaseCharacter* Interactor);

	UFUNCTION()
	virtual void OnEndFocus(class ABaseCharacter* Interactor);

private:
#pragma endregion 


	
public:	
	// Sets default values for this actor's properties
	ABaseInteractable();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
