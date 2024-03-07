// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter.h"

#include "../Interactables/BaseInteractable.h"
#include "../../BetrayalGameMode.h"
#include "../../BetrayalPlayerState.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "../Player/Player Components/InventoryComponent.h"
#include "../Player/Player Components/ObjectivesComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Net/UnrealNetwork.h"
#include "../../AI/Pawns/Monster.h"
#include "Perception/AIPerceptionStimuliSourceComponent.h"

APlayerCharacter::APlayerCharacter()
{
	BaseTurnRate = 45.0f;
	BaseLookUpRate = 45.0f;
	
	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComponent->SetupAttachment(GetMesh());
	CameraComponent->bUsePawnControlRotation = true;
	CameraComponent->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, FName("Head"));

	InteractableInFocus = nullptr;

	InventoryComponent = CreateDefaultSubobject<UInventoryComponent>(TEXT("Inventory Component"));

	ObjectivesComponent = CreateDefaultSubobject<UObjectivesComponent>(TEXT("Objectives Component"));
}

void APlayerCharacter::DebugInput()
{
	
}

void APlayerCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(APlayerCharacter, HeldItem);
	DOREPLIFETIME(APlayerCharacter, bWasItemUnequipped)
	DOREPLIFETIME(APlayerCharacter, bIsAttacking);
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

void APlayerCharacter::SelectSlot1()
{
	InventoryComponent->SelectSlot(0);

	if(HasAuthority())
	{
		EquipItem(InventoryComponent->GetItemInSlot(0).Actor.GetDefaultObject());
	}
	else
	{
		Server_EquipItem(InventoryComponent->GetItemInSlot(0).Actor.GetDefaultObject());
	}

	OnItemEquipped(InventoryComponent->GetSelectedSlot(), InventoryComponent->GetSelectedSlot().Item.Actor.GetDefaultObject());
}

void APlayerCharacter::SelectSlot2()
{
	InventoryComponent->SelectSlot(1);

	if(HasAuthority())
	{
		EquipItem(InventoryComponent->GetSelectedSlot().Item.Actor.GetDefaultObject());
	}
	else
	{
		Server_EquipItem(InventoryComponent->GetSelectedSlot().Item.Actor.GetDefaultObject());
	}

	OnItemEquipped(InventoryComponent->GetSelectedSlot(), InventoryComponent->GetSelectedSlot().Item.Actor.GetDefaultObject());
}

void APlayerCharacter::SelectSlot3()
{
	InventoryComponent->SelectSlot(2);

	if(HasAuthority())
	{
		EquipItem(InventoryComponent->GetSelectedSlot().Item.Actor.GetDefaultObject());
	}
	else
	{
		Server_EquipItem(InventoryComponent->GetSelectedSlot().Item.Actor.GetDefaultObject());
	}

	OnItemEquipped(InventoryComponent->GetSelectedSlot(), InventoryComponent->GetSelectedSlot().Item.Actor.GetDefaultObject());
	
}

void APlayerCharacter::SelectSlot4()
{
	InventoryComponent->SelectSlot(3);

	if(HasAuthority())
	{
		EquipItem(InventoryComponent->GetSelectedSlot().Item.Actor.GetDefaultObject());
	}
	else
	{
		Server_EquipItem(InventoryComponent->GetSelectedSlot().Item.Actor.GetDefaultObject());
	}


	OnItemEquipped(InventoryComponent->GetSelectedSlot(), InventoryComponent->GetSelectedSlot().Item.Actor.GetDefaultObject());

}

void APlayerCharacter::EquipItem(AItemActor* Item)
{
	if(!Item)
		return;

	if(HeldItem && Item->GetClass() == HeldItem->GetClass())
	{
		UnequipItem();
		HeldItem = nullptr;
		return;
	}
	else
	{
		UnequipItem();
	}
	
	//Spawn the item in the player's hand
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = this;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	if(AItemActor* ItemActor = GetWorld()->SpawnActor<AItemActor>(Item->GetClass(), SpawnParams))
	{
		ItemActor->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("ItemSocket"));
		ItemActor->SetActorRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
		ItemActor->SetActorRelativeRotation(FRotator(0.0f, 0.0f, 0.0f));
		HeldItem = ItemActor;
	}

	HeldItem->Server_SetCanPickup(false);
}

void APlayerCharacter::UnequipItem()
{
	if(!HeldItem)
		return;
	
	HeldItem->Destroy();

	bWasItemUnequipped = true;
}

void APlayerCharacter::Server_EquipItem_Implementation(AItemActor* Item)
{
	EquipItem(Item);
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

void APlayerCharacter::Server_UnequipItem_Implementation()
{
	UnequipItem();
}

void APlayerCharacter::TraceForInteractables()
{
	FVector TraceStart = CameraComponent->GetComponentLocation();
	FVector TraceEnd = TraceStart + (CameraComponent->GetForwardVector() * InteractDistance);
	FHitResult HitResult;
	FCollisionQueryParams TraceParams(FName(TEXT("")), false, this);

	if (GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd,ECC_PhysicsBody, TraceParams))
	{
		AActor* HitActor = HitResult.GetActor();
		
		if(!HitActor)
			return;
		
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
}

void APlayerCharacter::LocalInteract()
{
	if(InteractableInFocus)
		Server_Interact(UGameplayStatics::GetPlayerCharacter(GetWorld(),0), InteractableInFocus);
}

void APlayerCharacter::CycleSelectedMonster()
{
	Server_CycleSelectedMonster();
}

void APlayerCharacter::Server_CycleSelectedMonster_Implementation()
{
	const ABetrayalPlayerState* BetrayalPlayerState = GetPlayerState<ABetrayalPlayerState>();
	if (!BetrayalPlayerState->IsTraitor() || Monsters.Num() == 0)
		return;
	if (SelectedMonsterIndex == INDEX_NONE)
		return;
	if (SelectedMonsterIndex == Monsters.Num() - 1)
		SelectedMonsterIndex = 0;
	else
		SelectedMonsterIndex++;
}

void APlayerCharacter::SpawnMonster()
{
	Server_SpawnMonster();
}

void APlayerCharacter::Server_SpawnMonster_Implementation()
{
	const ABetrayalPlayerState* BetrayalPlayerState = GetPlayerState<ABetrayalPlayerState>();
	if (!BetrayalPlayerState->IsTraitor())
		return;
	if (Monsters[SelectedMonsterIndex].Count >= Monsters[SelectedMonsterIndex].MaxAmount)
		return;

	UWorld* World = GetWorld();
	if (!World)
		return;
	FVector Location = GetActorLocation();
	//Location.Y += 30;
	const FRotator Rotation = GetActorRotation();
	FActorSpawnParameters SpawnInfo;
	SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::Undefined;
	if (AMonster* Monster = World->SpawnActor<AMonster>(Monsters[SelectedMonsterIndex].Monster, Location, Rotation, SpawnInfo))
	{
		Monster->SpawnDefaultController();
		Monsters[SelectedMonsterIndex].Count++;
	}
}

void APlayerCharacter::Server_Interact_Implementation(class AActor* NewOwner, class ABaseInteractable* Interactable)
{
	if(Interactable)
		Interactable->OnInteract(NewOwner);
		
	
	NetMulticast_Interact(NewOwner,Interactable);
}

void APlayerCharacter::NetMulticast_Interact_Implementation(class AActor* NewOwner, class ABaseInteractable* Interactable)
{
	// if(NewOwner)
	// 	GEngine->AddOnScreenDebugMessage(-11, 2.0f, FColor::Green, "Interactable Owner: " + NewOwner->GetActorLabel());
}

void APlayerCharacter::BindMontageEvents()
{
	if(UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
	{
		AnimInstance->OnMontageEnded.AddDynamic(this, &APlayerCharacter::MontageEnded);
		AnimInstance->OnMontageStarted.AddDynamic(this, &APlayerCharacter::MontageStarted);
	}
}

void APlayerCharacter::MontageStarted(UAnimMontage* Montage)
{
	if(Montage == *AnimationMontages.Find(AM_Attack))
		bIsAttacking = true;

}

void APlayerCharacter::MontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if(Montage == *AnimationMontages.Find(AM_Attack))
		bIsAttacking = false;
}

void APlayerCharacter::Attack()
{
	OnAttack();
	
	Server_Attack();
}

void APlayerCharacter::Server_Attack_Implementation()
{
	NetMulticast_Attack();
}

void APlayerCharacter::NetMulticast_Attack_Implementation()
{
	if(!bIsAttacking && !HeldItem) // Don't attack if player has item on hand.
		PlayAnimMontage(AttackMontage, 1.0f, NAME_None);
}

void APlayerCharacter::HitDetect()
{
	FVector HitLocation = GetMesh()->GetSocketLocation("ItemSocket");

	TArray<FHitResult> HitResult;
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(this);
	
	GetWorld()->SweepMultiByChannel(HitResult, HitLocation, HitLocation, FQuat::Identity, ECC_Pawn, FCollisionShape::MakeSphere(50.0f), CollisionParams);

	for (auto Pawn : HitResult)
	{
		ABaseCharacter* Character = Cast<ABaseCharacter>(Pawn.GetActor());
		if(!Character)
			continue;
		
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, "Hit: " + Character->GetName());
	}
}

void APlayerCharacter::Server_HitDetect_Implementation()
{
	HitDetect();
}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	if(APlayerController* PlayerController = Cast<APlayerController>(Controller))
		if(UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
			Subsystem->AddMappingContext(DefaultMappingContext, 0);

	BindMontageEvents();
}

void APlayerCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	TraceForInteractables();
}

void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	if(UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(*InputAction.Find(IAV_Move), ETriggerEvent::Triggered, this, &ABaseCharacter::Move);

		EnhancedInputComponent->BindAction(*InputAction.Find(IAV_Look), ETriggerEvent::Triggered, this, &APlayerCharacter::TurnLook);
		EnhancedInputComponent->BindAction(*InputAction.Find(IAV_Look), ETriggerEvent::Triggered, this, &APlayerCharacter::UpDownLook);

		EnhancedInputComponent->BindAction(*InputAction.Find(IAV_Run), ETriggerEvent::Started, this, &APlayerCharacter::RunStart);
		EnhancedInputComponent->BindAction(*InputAction.Find(IAV_Run), ETriggerEvent::Completed, this, &APlayerCharacter::RunEnd);

		EnhancedInputComponent->BindAction(*InputAction.Find(IAV_Interact), ETriggerEvent::Started, this, &APlayerCharacter::LocalInteract);

		EnhancedInputComponent->BindAction(*InputAction.Find(IAV_Inventory1), ETriggerEvent::Started, this, &APlayerCharacter::SelectSlot1);
		EnhancedInputComponent->BindAction(*InputAction.Find(IAV_Inventory2), ETriggerEvent::Started, this, &APlayerCharacter::SelectSlot2);
		EnhancedInputComponent->BindAction(*InputAction.Find(IAV_Inventory3), ETriggerEvent::Started, this, &APlayerCharacter::SelectSlot3);
		EnhancedInputComponent->BindAction(*InputAction.Find(IAV_Inventory4), ETriggerEvent::Started, this, &APlayerCharacter::SelectSlot4);
		
		EnhancedInputComponent->BindAction(*InputAction.Find(IAV_TraitorCycleMonster), ETriggerEvent::Started, this, &APlayerCharacter::CycleSelectedMonster);
		EnhancedInputComponent->BindAction(*InputAction.Find(IAV_TraitorSpawnMonster), ETriggerEvent::Started, this, &APlayerCharacter::SpawnMonster);

		EnhancedInputComponent->BindAction(*InputAction.Find(IAV_Attack), ETriggerEvent::Started, this, &APlayerCharacter::Attack);
	}

	PlayerInputComponent->BindKey(EKeys::L, IE_Pressed, this, &APlayerCharacter::DebugInput);

	if (!HasAuthority())
		return;
	if (ABetrayalGameMode* BetrayalGameMode = GetWorld()->GetAuthGameMode<ABetrayalGameMode>())
		PlayerInputComponent->BindKey(EKeys::Y, IE_Pressed, BetrayalGameMode, &ABetrayalGameMode::SetNextStage);
}