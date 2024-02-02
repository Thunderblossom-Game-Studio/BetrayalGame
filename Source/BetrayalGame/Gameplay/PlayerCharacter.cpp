// Fill out your copyright notice in the Description page of Project Settings.


#include "../Gameplay/PlayerCharacter.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetSystemLibrary.h"

APlayerCharacter::APlayerCharacter()
{

	BaseTurnRate = 45.0f;
	BaseLookUpRate = 45.0f;
	
	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComponent->SetupAttachment(GetMesh());
	CameraComponent->bUsePawnControlRotation = true;
	
}

void APlayerCharacter::Quit() const
{
	UKismetSystemLibrary::QuitGame(this, nullptr, EQuitPreference::Quit, false);
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
	
	PlayerInputComponent->BindKey(EKeys::Escape, IE_Pressed, this, &APlayerCharacter::Quit);
}
