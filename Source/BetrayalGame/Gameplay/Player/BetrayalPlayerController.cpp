// Fill out your copyright notice in the Description page of Project Settings.


#include "BetrayalPlayerController.h"

#include "BetrayalGame/AI/Controllers/AIPlayerController.h"
#include "GameFramework/GameModeBase.h"
#include "Kismet/GameplayStatics.h"

void ABetrayalPlayerController::BeginPlay()
{
	Super::BeginPlay();

	TArray<AActor*> BotControllers;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), BotController, BotControllers);
	if (BotControllers.Num() == 0)
		return;
	AAIPlayerController* AICont = Cast<AAIPlayerController>(BotControllers[0]);
	if (!AICont)
		return;
	FTransform Transform = AICont->GetPawn()->GetTransform();
	AICont->GetPawn()->Destroy();
	AICont->Destroy();
	GetPawn()->SetActorTransform(Transform);
}

void ABetrayalPlayerController::Destroyed()
{	
	const int32 PlayerCount = UGameplayStatics::GetNumPlayerStates(GetWorld());
	const FVector Location = UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetSpawnLocation();
	const FRotator Rotation = FRotator::ZeroRotator;
	
	FActorSpawnParameters SpawnParams;
	
	AAIPlayerController* AIPlayerController = GetWorld()->SpawnActor<AAIPlayerController>(BotController, Location, Rotation);
	if (AIPlayerController)
	{
		GetWorld()->GetAuthGameMode()->RestartPlayerAtPlayerStart(AIPlayerController, GetPawn());			
		AIPlayerController->EnableAIPlayer();
	}
	
	Super::Destroyed();
}
