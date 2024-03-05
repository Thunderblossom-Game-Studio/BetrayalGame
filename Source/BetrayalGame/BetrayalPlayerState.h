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

#pragma region Lobby
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player State", Replicated)
	bool bIsReady = false;
	
#pragma endregion
public:
	
	UFUNCTION()
	void OnRep_IsTraitor();

	UFUNCTION()
	bool IsTraitor() const { return bIsTraitor; }

	UFUNCTION(BlueprintCallable)
	void SetIsTraitor(bool bNewIsTraitor) { bIsTraitor = bNewIsTraitor; }

#pragma region Lobby

	UFUNCTION()
	bool IsReady() const { return bIsReady; }

	UFUNCTION()
	void SetIsReady(bool bNewIsReady) { bIsReady = bNewIsReady; }

#pragma endregion
};



