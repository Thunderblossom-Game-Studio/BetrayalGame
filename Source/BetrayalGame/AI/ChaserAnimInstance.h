// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "ChaserAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class BETRAYALGAME_API UChaserAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	UChaserAnimInstance();

	virtual void NativeInitializeAnimation() override;

#pragma region Animation Conditions
// Exposed Variables
protected:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Animation | Movement", meta = (AllowPrivateAccess = "true"))
	float MoveSpeed;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Animation | Movement", meta = (AllowPrivateAccess = "true"))
	bool bRunning;

// Exposed Functions
public:
	UFUNCTION(BlueprintCallable, Category = "Animation | Updating")
	void CustomAnimUpdate();

// Cached Variables	
private:	
	UPROPERTY()
	class AChaser* Chaser;
	
#pragma endregion
	
};