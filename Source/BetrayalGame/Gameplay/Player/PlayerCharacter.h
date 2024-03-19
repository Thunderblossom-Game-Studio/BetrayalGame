// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../Player/Player Components/InventoryComponent.h"
#include "../Player/BaseCharacter.h"
#include "../Interactables/Items/ItemActor.h"
#include "BetrayalGame/Gameplay/Chestlight.h"
#include "PlayerCharacter.generated.h"

struct FInputActionValue;
class UInputAction;
class UInputMappingContext;

UENUM()
enum EInputActionValue
{
	IAV_None UMETA(DisplayName = "None"),
	IAV_Move UMETA(DisplayName = "Move"),
	IAV_Look UMETA(DisplayName = "Look"),
	IAV_Run UMETA(DisplayName = "Run"),
	IAV_Interact UMETA(DisplayName = "Interact"),
	IAV_Inventory1 UMETA(DisplayName = "Inventory1"),
	IAV_Inventory2 UMETA(DisplayName = "Inventory2"),
	IAV_Inventory3 UMETA(DisplayName = "Inventory3"),
	IAV_Inventory4 UMETA(DisplayName = "Inventory4"),
	IAV_TraitorCycleMonster UMETA(DisplayName = "TraitorCycleMonster"),
	IAV_TraitorSpawnMonster UMETA(DisplayName = "TraitorSpawnMonster"),
	IAV_Attack UMETA(DisplayName = "Attack"),
	IAV_DropItem UMETA(DisplayName = "DropItem"),
	IAV_ToggleLight UMETA(DisplayName = "ToggleLight"),
};

UENUM()
enum EAnimationMontages
{
	AM_None UMETA(DisplayName = "None"),
	AM_Attack UMETA(DisplayName = "Attack"),
};

USTRUCT(Blueprintable)
struct FMonsterSpawnInfo
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Monster")
	TSubclassOf<class AMonster> Monster;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Monster")
	int MaxAmount = 1;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Monster")
	int Count = 0;
};

UCLASS()
class BETRAYALGAME_API APlayerCharacter : public ABaseCharacter
{
	GENERATED_BODY()
	
public:

	APlayerCharacter();

	void DebugInput();
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
#pragma region Animation
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category= "Player|Animation")
	TMap<TEnumAsByte<EAnimationMontages>, UAnimMontage*> AnimationMontages;
	
	UFUNCTION()
	void BindMontageEvents();
	
	UFUNCTION()
	void MontageStarted(UAnimMontage* Montage);

	UFUNCTION()
	void MontageEnded(UAnimMontage* Montage, bool bInterrupted);
	
#pragma endregion 
	
#pragma region Camera
public:
	UPROPERTY(EditAnywhere,Category="Player|Camera")
	class UCameraComponent* CameraComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player|Camera")
	float BaseTurnRate;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player|Camera")
	float BaseLookUpRate;

	void TurnLook(const FInputActionValue& Value);

	void UpDownLook(const FInputActionValue& Value);
	
private:
#pragma endregion 
	
#pragma region Input
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player|Input", meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player|Input", meta = (AllowPrivateAccess = "true"))
	TMap<TEnumAsByte<EInputActionValue>, UInputAction*> InputAction;

private:
#pragma endregion

#pragma region Movement
public:
	virtual void Move(const FInputActionValue& Value) override;
	
	UFUNCTION(Server, Reliable)
	void RunStart();

	UFUNCTION(Server, Reliable)
	void RunEnd();
private:
#pragma endregion

#pragma region Inventory
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player|Inventory")
	class USphereComponent* ItemDropLocation;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player|Inventory")
	class UInventoryComponent* InventoryComponent;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadOnly, Category = "Player|Inventory")
	AItemActor* HeldItem;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadOnly, Category = "Player|Inventory")
	bool bWasItemUnequipped;
	
	UFUNCTION()
	void SelectSlot1();
	
	UFUNCTION()
	void SelectSlot2();
	
	UFUNCTION()
	void SelectSlot3();
	
	UFUNCTION()
	void SelectSlot4();

	UFUNCTION()
	void EquipItem(int SlotID);
	UFUNCTION(Server, Reliable)
	void Server_EquipItem(int SlotID);

	UFUNCTION()
	void UnequipItem();
	UFUNCTION(Server, Reliable)
	void Server_UnequipItem();

	UFUNCTION()
	void DropHeldItem();
	UFUNCTION(Server, Reliable)
	void Server_DropHeldItem();
	
	UFUNCTION()
	void DropItem(FInventorySlot Slot);
	UFUNCTION(BlueprintCallable, Server, Reliable)
	void Server_DropItem(FInventorySlot Slot);
	
	UFUNCTION()
	void DropAllItems();
	UFUNCTION(BlueprintCallable, Server, Reliable)
	void Server_DropAllItems();
	
	UFUNCTION(BlueprintImplementableEvent, Category = "Player|Inventory")
	void OnSlotSelected(FInventorySlot Slot);

	UFUNCTION(BlueprintImplementableEvent, Category = "Player|Inventory")
	void OnItemRemovedFromInventory(FInventorySlot Slot);
	
private:	
#pragma endregion 

#pragma region Interaction
public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player|Interaction")
	class ABaseInteractable* InteractableInFocus;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player|Interaction")
	float InteractDistance = 600.0f;
	
	void TraceForInteractables();

	UFUNCTION(BlueprintImplementableEvent, Category = "Player|Interaction")
	void OnItemPickedUp(FInventorySlot Slot);
	
	void LocalInteract();
	
	UFUNCTION(Server, Reliable)
	void Server_Interact(class AActor* NewOwner, class ABaseInteractable* Interactable);

	UFUNCTION(NetMulticast, Reliable)
	void NetMulticast_Interact(class AActor* NewOwner, class ABaseInteractable* Interactable);

private:
#pragma endregion 

#pragma region Traitor Actions
protected:
	UFUNCTION()
	void CycleSelectedMonster();
	UFUNCTION(Server, Reliable)
	void Server_CycleSelectedMonster();
	
	UFUNCTION()
	void SpawnMonster();
	UFUNCTION(Server, Reliable)
	void Server_SpawnMonster();

	UPROPERTY(EditDefaultsOnly, Category = "Player|Traitor", meta = (AllowPrivateAccess))
	TArray<FMonsterSpawnInfo> Monsters;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player|Traitor", meta = (AllowPrivateAccess = "true"))
	int SelectedMonsterIndex = 0;
	
#pragma endregion
	
#pragma region Objectives
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player|Objectives")
	class UObjectivesComponent* ObjectivesComponent;

	
private:
#pragma endregion

#pragma region Combat System
protected:
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite, Category = "Player|Combat")
	bool bIsAttacking = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player|Combat")
	UAnimMontage* AttackMontage;
	
public:
	UFUNCTION()
	void Attack();

	UFUNCTION(Server, Reliable)
	void Server_Attack();

	UFUNCTION(NetMulticast, Reliable)
	void NetMulticast_Attack();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Player|Combat")
	void OnAttack();



private:
#pragma endregion

#pragma region Chestlight
	UPROPERTY()
	AChestlight* Chestlight;

	UFUNCTION()
	void ToggleLight();
	
#pragma endregion 
                                  
protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaSeconds) override;

	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
};
