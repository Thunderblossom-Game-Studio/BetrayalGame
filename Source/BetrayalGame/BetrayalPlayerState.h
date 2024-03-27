// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "Gameplay/Player/PlayerCharacter.h"
#include "BetrayalPlayerState.generated.h"


UENUM()
enum EControlState
{
	CS_None UMETA(DisplayName = "None"),
	CS_Player UMETA(DisplayName = "Player"),
	CS_AI UMETA(DisplayName = "AI"),
	CS_Spectator UMETA(DisplayName = "Spectator")
};


UCLASS()
class BETRAYALGAME_API ABetrayalPlayerState : public APlayerState
{
	GENERATED_BODY()
	
	virtual void CopyProperties(APlayerState* PlayerState) override;

	virtual void OverrideWith(APlayerState* PlayerState) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;


protected:

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "State")
	FName DisplayName = "Dave";
	
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "State")
	TEnumAsByte<EControlState> ControlState = CS_Player;
	
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "State")
	APlayerCharacter* ControlledCharacter;

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "State")
	TSubclassOf<APlayerCharacter> DefaultCharacterBlueprint;
	
	UFUNCTION(BlueprintCallable, Category = "State")
	void SetControlState(EControlState NewControlState) { ControlState = NewControlState; }

	UFUNCTION(BlueprintPure, Category = "State")
	EControlState GetControlState() const { return ControlState; }

	UFUNCTION(BlueprintCallable, Category = "State")
	void SetDisplayName(FName NewDisplayName) { DisplayName = NewDisplayName; }
	
	UFUNCTION(BlueprintPure, Category = "State")
	FName GetDisplayName() const { return DisplayName; }

	UFUNCTION()
	void SetControlledCharacter(APlayerCharacter* NewControlledCharacter) { ControlledCharacter = NewControlledCharacter; }
	UFUNCTION(Blueprintable, Server, Reliable)
	void Server_SetControlledCharacter(APlayerCharacter* NewControlledCharacter);

	UFUNCTION(BlueprintCallable, Category = "State")
	void SelectCharacter(TSubclassOf<APlayerCharacter> NewControlledCharacter);
	
	
	UFUNCTION(BlueprintPure, Category = "State")
	APlayerCharacter* GetControlledCharacter() const { return ControlledCharacter; }

	UFUNCTION(BlueprintCallable, Category = "State")
	void ChangeCharacter(TSubclassOf<APlayerCharacter> NewControlledCharacter);
	UFUNCTION(BlueprintCallable, Server, Reliable , Category = "State")
	void Server_ChangeCharacter(TSubclassOf<APlayerCharacter> NewControlledCharacter);

	UFUNCTION(BlueprintImplementableEvent, Category = "State")
	void OnCharacterChanged();
	
#pragma region AI


private:
#pragma endregion
	
#pragma region Gameplay
protected:
	UPROPERTY(VisibleAnywhere, ReplicatedUsing=OnRep_IsTraitor, BlueprintReadOnly, Category = "State|Gameplay")
	bool bIsTraitor = false;

public:
	UFUNCTION()
	void OnRep_IsTraitor();
	
	UFUNCTION(BlueprintImplementableEvent)
	void OnIsTraitorChanged(bool bNewIsTraitor);
	

	UFUNCTION(BlueprintPure, Category = "State|Gameplay")
	bool IsTraitor() const { return bIsTraitor; }

	UFUNCTION(BlueprintCallable , Category = "State|Gameplay")
	void SetIsTraitor(bool bNewIsTraitor) { bIsTraitor = bNewIsTraitor; }
	
private:
#pragma endregion
	
#pragma region Lobby

#pragma region ReadyState
protected:
	UPROPERTY(VisibleAnywhere)
	bool bIsReady = false;

public:
	UFUNCTION(BlueprintCallable, Category = "Lobby|ReadyState")
	bool IsReady() const { return bIsReady; }

	UFUNCTION(BlueprintCallable, Server, Reliable, Category = "Lobby|ReadyState")
	void SetIsReady(bool bReady);

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_SetIsReady(bool bReady, ABetrayalPlayerState* Player);

private:
#pragma endregion

	

#pragma endregion
	
	
	
};



