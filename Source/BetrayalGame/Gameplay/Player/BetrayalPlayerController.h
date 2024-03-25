// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PlayerCharacter.h"
#include "BetrayalGame/BetrayalPlayerState.h"
#include "GameFramework/PlayerController.h"
#include "BetrayalPlayerController.generated.h"

class ABetrayalPlayerState;
class APlayerCharacter;

UCLASS()
class BETRAYALGAME_API ABetrayalPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	

	void BeginPlay() override;

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	FTransform DestroyedTransform; //TODO - Make protected later

#pragma region References 
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Controller|References")
	TSubclassOf<APlayerCharacter> DefaultCharacterBlueprint;
	
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Controller|References")
	APlayerCharacter* ControlledCharacter;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Controller|References")
	ABetrayalPlayerState* BetrayalPlayerState;

public:
	UFUNCTION(BlueprintPure, Category = "Controller|References")
	APlayerCharacter* GetControlledCharacter() const { return ControlledCharacter; }

	UFUNCTION(BlueprintCallable, Category = "Controller|References")
	void SetControlledCharacter(APlayerCharacter* NewControlledCharacter) { ControlledCharacter = NewControlledCharacter; }

	UFUNCTION(Server, Reliable)
	void Server_SetControlledCharacter(APlayerCharacter* NewControlledCharacter);
	
	UFUNCTION(BlueprintPure, Category = "Controller|References")
	ABetrayalPlayerState* GetBetrayalPlayerState() const { return BetrayalPlayerState; }

	UFUNCTION(BlueprintCallable, Category = "Controller|References")
	void SetBetrayalPlayerState(ABetrayalPlayerState* NewBetrayalPlayerState) { BetrayalPlayerState = NewBetrayalPlayerState; }

	UFUNCTION()
	void InitializeReferences();
	
	UFUNCTION(Server, Reliable)
	void Server_InitializeReferences();

	UFUNCTION(BlueprintImplementableEvent, Category = "Controller|References")
	void OnReferenceInitialized(APlayerCharacter* ControllerCharacter, ABetrayalPlayerState* State);
	
	UFUNCTION(Server, Reliable)
	void Server_OnReferenceInitialized();
		
private:
#pragma endregion

#pragma region Input
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Controller|Input", meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Controller|Input", meta = (AllowPrivateAccess = "true"))
	TMap<TEnumAsByte<EInputActionValue>, UInputAction*> InputAction;
	
#pragma endregion 
	UFUNCTION(BlueprintCallable, Category = "Controller|Gameplay")
	void SpawnPlayerCharacter();

	UFUNCTION(Server, Reliable)
	void Server_SpawnPlayerCharacter();
	
};
