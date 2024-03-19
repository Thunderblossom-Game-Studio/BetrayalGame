// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Chestlight.generated.h"

UCLASS()
class BETRAYALGAME_API AChestlight : public AActor
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	class USpotLightComponent* Light;

	UPROPERTY(EditAnywhere, Category = "Chestlight")
	float Intensity = 1000.0f;


#pragma region Battery
protected:
	UPROPERTY(EditAnywhere, Category = "Chestlight|Battery")
	float MaxBatteryCharge = 100.0f;

	UPROPERTY(VisibleAnywhere, Category = "Chestlight|Battery")
	float CurrentBatteryCharge = MaxBatteryCharge;

	UPROPERTY(VisibleAnywhere, Category = "Chestlight|Battery")
	bool bIsBatteryEmpty = false;
	
public:
	UFUNCTION(BlueprintCallable, Category = "Chestlight|Battery")
	void DrainBattery(float DrainAmount);

	UFUNCTION(BlueprintCallable, Category = "Chestlight|Battery")
	void RechargeBattery(float RechargeAmount);

	UFUNCTION(BlueprintPure, Category = "Chestlight|Battery")
	float GetBatteryCharge() const { return CurrentBatteryCharge; }

	UFUNCTION(BlueprintPure, Category = "Chestlight|Battery")
	bool IsBatteryEmpty() const { return bIsBatteryEmpty; }
	
private:
#pragma endregion 
	
public:	
	// Sets default values for this actor's properties
	AChestlight();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
