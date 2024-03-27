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
	
	virtual void PawnLeavingGame() override;

#pragma region References 
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Controller|References")
	TSubclassOf<APlayerCharacter> DefaultCharacterBlueprint;
	
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Controller|References")
	ABetrayalPlayerState* BetrayalPlayerState;

public:
	UFUNCTION(BlueprintPure, Category = "Controller|References")
	ABetrayalPlayerState* GetBetrayalPlayerState() const { return BetrayalPlayerState; }

	UFUNCTION(BlueprintCallable, Category = "Controller|References")
	void SetBetrayalPlayerState(ABetrayalPlayerState* NewBetrayalPlayerState) { BetrayalPlayerState = NewBetrayalPlayerState; }

	UFUNCTION()
	void InitBetrayalPlayerState();
	
	UFUNCTION(Server, Reliable)
	void Server_InitializeReferences();

	UFUNCTION(BlueprintNativeEvent, Category = "Controller|References")
	void OnReferenceInitialized(APlayerCharacter* ControlledPlayerCharacter, ABetrayalPlayerState* State);
	
	UFUNCTION(Server, Reliable, Category = "Controller|References")
	void Server_OnReferenceInitialized(APlayerCharacter* ControlledPlayerCharacter, ABetrayalPlayerState* State);
		
private:
#pragma endregion

#pragma region Input
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Controller|Input", meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Controller|Input", meta = (AllowPrivateAccess = "true"))
	TMap<TEnumAsByte<EInputActionValue>, UInputAction*> InputAction;
	
#pragma endregion 

#pragma region AI Integration
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Bots)
	// The A.I player controller.
	TSubclassOf<class AController> BotController;

private:
	// Starts the process of choosing a pawn.
	void SetupPlayerCharacter();

	// Checks if the lobbies full and if it is the controller replaces a bot controller, otherwise it creates a new character.
	void DetermineNewOrReplaceCharacter();
	
	// Sets up the pawns input system.
	void SetupControllerInput();
	
	// Creates a new player character
	void InitializeNewCharacter();
	
	// Replaces the player with an AI controller.
	void ReplacePlayerWithBot();
	
	// Takes control of an AI controller's pawn.
	void ReplaceBotWithPlayer();
	
#pragma endregion

	UFUNCTION(BlueprintCallable, Category = "Controller|Gameplay")
	void SpawnPlayerCharacter();

	UFUNCTION(Server, Reliable)
	void Server_SpawnPlayerCharacter();
	
};
