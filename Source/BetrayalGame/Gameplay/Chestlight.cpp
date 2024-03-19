// Fill out your copyright notice in the Description page of Project Settings.


#include "Chestlight.h"

#include "Components/SpotLightComponent.h"

// Sets default values
AChestlight::AChestlight()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;

	Light = CreateDefaultSubobject<USpotLightComponent>(TEXT("Light"));
	Light->SetupAttachment(RootComponent);
	
}

// Called when the game starts or when spawned
void AChestlight::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AChestlight::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

