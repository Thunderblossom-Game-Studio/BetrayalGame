// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "BetrayalPlayerState.generated.h"


/**
 * 
 */
UCLASS()
class BETRAYALGAME_API ABetrayalPlayerState : public APlayerState
{
	GENERATED_BODY()
	
	virtual void CopyProperties(APlayerState* PlayerState) override;

	virtual void OverrideWith(APlayerState* PlayerState) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
protected:
	UPROPERTY(VisibleAnywhere, ReplicatedUsing=OnRep_IsTraitor, BlueprintReadOnly, Category = "Player State")
	bool bIsTraitor = false;

	UPROPERTY(VisibleAnywhere)
	bool bIsReady = false;
	
public:
	UFUNCTION()
	void OnRep_IsTraitor();

	
	UFUNCTION(BlueprintImplementableEvent)
	void OnIsTraitorChanged(bool bNewIsTraitor);
	
	UFUNCTION()
	bool IsTraitor() const { return bIsTraitor; }

	UFUNCTION(BlueprintCallable)
	void SetIsTraitor(bool bNewIsTraitor) { bIsTraitor = bNewIsTraitor; }

#pragma region Lobby
	UFUNCTION(BlueprintCallable, Category = "Lobby")
	bool IsReady() const { return bIsReady; }

	UFUNCTION(BlueprintCallable, Server, Reliable, Category = "Lobby")
	void SetIsReady(bool bReady);

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_SetIsReady(bool bReady, ABetrayalPlayerState* Player);

#pragma endregion
};



