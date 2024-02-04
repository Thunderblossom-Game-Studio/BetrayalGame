// Fill out your copyright notice in the Description page of Project Settings.


#include "../Gameplay/PlayerCharacter.h"

#include "BaseInteractable.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Net/UnrealNetwork.h"

APlayerCharacter::APlayerCharacter()
{

	BaseTurnRate = 45.0f;
	BaseLookUpRate = 45.0f;
	
	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComponent->SetupAttachment(GetMesh());
	CameraComponent->bUsePawnControlRotation = true;

	//SetReplicates(true);
	
}

void APlayerCharacter::NetDebugging()
{
	if(HasAuthority())
	{
		GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::White, "Server interactable in focus: " + InteractableInFocus->GetName());
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::White, "Client interactable in focus: " + InteractableInFocus->GetName());
	}
	
}

void APlayerCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APlayerCharacter, InteractableInFocus);
	
}





void APlayerCharacter::TurnLook(const FInputActionValue& Value)
{
	const FVector2D LookInput = Value.Get<FVector2D>();

	if(!Controller)
		return;
	
	AddControllerYawInput(LookInput.X * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void APlayerCharacter::UpDownLook(const FInputActionValue& Value)
{
	const FVector2D LookInput = Value.Get<FVector2D>();

	if(!Controller)
		return;
	
	const float NewRotation = LookInput.Y * BaseLookUpRate * GetWorld()->GetDeltaSeconds();

		
	AddControllerPitchInput(-NewRotation ); // Value is negated because it's inverted
}

void APlayerCharacter::Move(const FInputActionValue& Value)
{
	Super::Move(Value);

	const FVector2D MovementInput = Value.Get<FVector2D>();

	if(bIsRunning)
		GetCharacterMovement()->MaxWalkSpeed = RunSpeed;
	else
		GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
	
	if (Controller != nullptr)
	{
		// Find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// Forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	
		// Right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);


		
		AddMovementInput(ForwardDirection, MovementInput.Y);
		AddMovementInput(RightDirection , MovementInput.X);
	}
}

void APlayerCharacter::RunStart()
{
	bIsRunning = true;
}

void APlayerCharacter::RunEnd()
{
	bIsRunning = false;
}


void APlayerCharacter::Server_TraceForInteractables_Implementation()
{
	FVector TraceStart = CameraComponent->GetComponentLocation();
	FVector TraceEnd = TraceStart + (CameraComponent->GetForwardVector() * 1000.0f); // TODO - Make 1000.0f a variable

	FHitResult HitResult;
	FCollisionQueryParams TraceParams(FName(TEXT("")), false, GetOwner());

	if (GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd,ECC_PhysicsBody, TraceParams))
	{
		AActor* HitActor = HitResult.GetActor();
		
		if(!HitActor)
			return;

		if(HitActor->ActorHasTag("Interactable"))
		{
			InteractableInFocus = Cast<ABaseInteractable>(HitActor);
			GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Green, InteractableInFocus->GetName() + " detected by line trace!");
			
		}
		else if(!HitActor->ActorHasTag("Interactable"))
		{
			InteractableInFocus = nullptr;
		}
	}
}

void APlayerCharacter::TraceForActors()
{
	if(HasAuthority())
	{
		// If we are the server, we can just call the server function directly
		Server_TraceForInteractables();
		
	}
	else
	{
		// If we are a client, we need to call the server function through the player controller
		Server_TraceForInteractables();
		
	}

	FVector TraceStart = CameraComponent->GetComponentLocation();
	FVector TraceEnd = TraceStart + (CameraComponent->GetForwardVector() * 1000.0f); // TODO - Make 1000.0f a variable

	DrawDebugLine(GetWorld(), TraceStart, TraceEnd, FColor::Green, false, 1.0f, 0, 1.0f);
	
	
}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	if(APlayerController* PlayerController = Cast<APlayerController>(Controller))
		if(UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
}

void APlayerCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	TraceForActors();
}

void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if(UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ABaseCharacter::Move);

		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &APlayerCharacter::TurnLook);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &APlayerCharacter::UpDownLook);

		EnhancedInputComponent->BindAction(RunAction, ETriggerEvent::Started, this, &APlayerCharacter::RunStart);
		EnhancedInputComponent->BindAction(RunAction, ETriggerEvent::Completed, this, &APlayerCharacter::RunEnd);
	}
}
