// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "GameFramework/SaveGame.h"
#include "BetrayalGameInstance.generated.h"

#pragma region Save/Load
USTRUCT(BlueprintType)
struct FPlayerProfile
{
	GENERATED_BODY()
	FString PlayerName;
};
#pragma endregion

/**
 * 
 */
UCLASS()
class BETRAYALGAME_API UBetrayalGameInstance : public UGameInstance
{
	GENERATED_BODY()

#pragma region General
	UBetrayalGameInstance(const FObjectInitializer& ObjectInitializer);
	virtual void Init() override;
	
	UFUNCTION(BlueprintCallable, Category = "General")
	void QuitGame();
#pragma endregion
	
public:
#pragma region Save/Load
private:
	UPROPERTY()
	USaveGame* SG_PlayerProfile;

	UPROPERTY()
	FString SaveSlotName = "UserProfile";

	UPROPERTY()
	FPlayerProfile PlayerProfileData;

public:
	UFUNCTION(BlueprintCallable, Category = "Save/Load")
	void SavePlayerProfile();

	UFUNCTION(BlueprintCallable, Category = "Save/Load")
	void LoadPlayerProfile();

	UFUNCTION(BlueprintCallable, Category = "Save/Load")
	void CheckPlayerProfile();
	
#pragma endregion
#pragma region Networking	
	UPROPERTY(EditAnywhere, Category = "Networking")
	FName LevelToLoad = "";

#pragma endregion
	
};
