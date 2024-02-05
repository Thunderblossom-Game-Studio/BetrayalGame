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
	
protected:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Monster | Animation", meta = (AllowPrivateAccess = "true"))
	float MoveSpeed;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Monster | Animation", meta = (AllowPrivateAccess = "true"))
	bool bChasing;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Monster | Animation", meta = (AllowPrivateAccess = "true"))
	class AChaser* Pawn;

public:
	UFUNCTION(BlueprintCallable, Category = "Monster | Animation")
	void CustomAnimUpdate();
	
};