// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputActionValue.h"
#include "../Gameplay/BaseCharacter.h"
#include "PlayerCharacter.generated.h"

struct FInputActionValue;
class UInputAction;
class UInputMappingContext;

UCLASS()
class BETRAYALGAME_API APlayerCharacter : public ABaseCharacter
{
	GENERATED_BODY()
	
public:

	APlayerCharacter();

	virtual void NetDebugging() override;

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
	
#pragma region Camera
public:
	UPROPERTY(EditAnywhere,Category="Components|Camera")
	class UCameraComponent* CameraComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components|Camera")
	float BaseTurnRate;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components|Camera")
	float BaseLookUpRate;

	void TurnLook(const FInputActionValue& Value);

	void UpDownLook(const FInputActionValue& Value);
	
private:
#pragma endregion 
	
#pragma region Input
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player|Input", meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player|Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player|Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* LookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player|Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* RunAction;
private:
#pragma endregion

#pragma region Movement
public:
	virtual void Move(const FInputActionValue& Value) override;

	void RunStart();

	void RunEnd();
private:
#pragma endregion

#pragma region Inventory
public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player|Inventory")
	class UInventoryComponent* InventoryComponent;

private:	
#pragma endregion 

#pragma region Interaction
public:

	UPROPERTY(Replicated)
	class ABaseInteractable* InteractableInFocus;

	UFUNCTION(Server, Reliable)
	void Server_TraceForInteractables();

	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void TraceForActors();
	
	

private:
#pragma endregion 

	
protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaSeconds) override;

	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
};
