// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimExecutionContext.h"
#include "Animation/AnimNodeReference.h"
#include "MaulerAnimInstance.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class BETRAYALGAME_API UMaulerAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	UMaulerAnimInstance();

	virtual void NativeInitializeAnimation() override;

#pragma region Animation Conditions
	// Exposed Variables
protected:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Monster | Animation", meta = (AllowPrivateAccess = "true"))
	float MoveSpeed;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Monster | Animation", meta = (AllowPrivateAccess = "true"))
	bool bSprinting;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Monster | Animation", meta = (AllowPrivateAccess = "true"))
	bool bAttack;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Monster | Animation", meta = (AllowPrivateAccess = "true"))
	class AMauler* Mauler;

	// Exposed Functions
public:
	UFUNCTION(BlueprintCallable, Category = "Monster | Animation")
	void CustomAnimUpdate();
	UFUNCTION(Category="StateNodeFunctions", BlueprintCallable, meta=(BlueprintThreadSafe))
	void ResetAttack(const FAnimUpdateContext& Context, const FAnimNodeReference& Node);
	
	UFUNCTION()
	bool IsAttacking() const { return bAttack; }
	
#pragma endregion
};
