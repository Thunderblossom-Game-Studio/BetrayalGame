// Fill out your copyright notice in the Description page of Project Settings.


#include "BTDecorator_IsPlayer.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"
#include "BetrayalGame/Gameplay/Player/PlayerCharacter.h"


UBTDecorator_IsPlayer::UBTDecorator_IsPlayer(FObjectInitializer const& ObjectInitializer)
{
	NodeName = TEXT("Is Player");
	BlackboardKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UBTDecorator_IsPlayer, BlackboardKey), AActor::StaticClass());
}

bool UBTDecorator_IsPlayer::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp,
                                                       uint8* NodeMemory) const
{
	AAIController* Controller = OwnerComp.GetAIOwner();
	if (!Controller)
		return false;
	const UBlackboardComponent* Blackboard = Controller->GetBlackboardComponent();
	if (!Blackboard)
		return false;	
	UObject* KeyValue = Blackboard->GetValue<UBlackboardKeyType_Object>(BlackboardKey.GetSelectedKeyID());
	APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(KeyValue);
	if (!PlayerCharacter)
		return false;
	const APlayerController* PlayerController = PlayerCharacter->GetController<APlayerController>();
	if (!PlayerController)
		return false;	
	return true;
}
