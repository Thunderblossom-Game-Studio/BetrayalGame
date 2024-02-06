// Fill out your copyright notice in the Description page of Project Settings.


#include "../Gameplay/PlayerCharacter.h"

#include "BaseInteractable.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Net/UnrealNetwork.h"

APlayerCharacter::APlayerCharacter()
{	
	BaseTurnRate = 45.0f;
	BaseLookUpRate = 45.0f;
	
	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComponent->SetupAttachment(GetMesh());
	CameraComponent->bUsePawnControlRotation = true;

	CameraComponent->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, FName("Head"));

	InteractableInFocus = nullptr;

	bReplicates = true;
}

void APlayerCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
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

	if (bIsRunning)		
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

void APlayerCharacter::RunStart_Implementation()
{
	bIsRunning = true;
	GetCharacterMovement()->MaxWalkSpeed = RunSpeed;
}

void APlayerCharacter::RunEnd_Implementation()
{
	bIsRunning = false;
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
}

void APlayerCharacter::TraceForInteractables()
{
	FVector TraceStart = CameraComponent->GetComponentLocation();
	FVector TraceEnd = TraceStart + (CameraComponent->GetForwardVector() * 1000.0f); // TODO - Make 1000.0f a variable
	FHitResult HitResult;
	FCollisionQueryParams TraceParams(FName(TEXT("")), false, this);

	if (GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd,ECC_PhysicsBody, TraceParams))
	{
		AActor* HitActor = HitResult.GetActor();
		
		if(HitActor->Implements<UInteractable>() && HitActor != InteractableInFocus)
		{
			InteractableInFocus = Cast<ABaseInteractable>(HitActor);
		}
		else if (!HitActor->Implements<UInteractable>())
		{
			InteractableInFocus = nullptr;
		}
	}
	else
	{
		InteractableInFocus = nullptr;
	}
	
	DrawDebugLine(GetWorld(),TraceStart,TraceEnd,FColor::Blue,false,0.1f,-1,1.0f);
}

void APlayerCharacter::LocalInteract()
{
	if(InteractableInFocus)
	{
		Server_Interact(UGameplayStatics::GetPlayerCharacter(GetWorld(),0), InteractableInFocus);
	}
}

void APlayerCharacter::Server_Interact_Implementation(class AActor* NewOwner, class ABaseInteractable* Interactable)
{
	if(Interactable)
		Interactable->OnInteract(NewOwner);
	
	GEngine->AddOnScreenDebugMessage(-10, 2.0f, FColor::Green, "Owner: " + NewOwner->GetActorLabel());
	
	NetMulticast_Interact(NewOwner,Interactable);
}

void APlayerCharacter::NetMulticast_Interact_Implementation(class AActor* NewOwner, class ABaseInteractable* Interactable)
{
	// if(NewOwner)
	// 	GEngine->AddOnScreenDebugMessage(-11, 2.0f, FColor::Green, "Interactable Owner: " + NewOwner->GetActorLabel());
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

	if (Controller && Controller->IsLocalPlayerController())
	{
		if(HasAuthority())
		{
			if(!InteractableInFocus)
				GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Red, "InteractableInFocus is null");
			else
				GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Green, "InteractableInFocus is not null");
		}
		else
		{
			if(!InteractableInFocus)
				GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Red, "InteractableInFocus is null");
			else
				GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Green, "InteractableInFocus is not null");
		}
	}


	TraceForInteractables();
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

		EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Started, this, &APlayerCharacter::LocalInteract);
	}
}

void APlayerCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}
