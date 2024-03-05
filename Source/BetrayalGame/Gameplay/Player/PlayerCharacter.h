// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../Player/Player Components/InventoryComponent.h"
#include "../Player/BaseCharacter.h"
#include "../Interactables/Items/ItemActor.h"
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
	IAV_TraitorSpawnMonster UMETA(DisplayName = "TraitorSpawnMonster")
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
	class UInventoryComponent* InventoryComponent;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadOnly, Category = "Player|Inventory")
	AItemActor* HeldItem;

	UFUNCTION()
	void SelectSlot1();
	
	UFUNCTION()
	void SelectSlot2();
	
	UFUNCTION()
	void SelectSlot3();
	
	UFUNCTION()
	void SelectSlot4();

	UFUNCTION()
	void EquipItem(AItemActor* Item);
	UFUNCTION(Server, Reliable)
	
	void Server_EquipItem(AItemActor* Item);

	UFUNCTION()
	void UnequipItem();
	
	UFUNCTION(Server, Reliable)
	void Server_UnequipItem();

	UFUNCTION(BlueprintImplementableEvent, Category = "Player|Inventory")
	void OnItemEquipped(AItemActor* Item, int SlotID);

	UFUNCTION(BlueprintImplementableEvent, Category = "Player|Inventory")
	void OnItemUnequipped();
	
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
	void OnItemPickedUp(AItemActor* Item);
	
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
	
protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaSeconds) override;

	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
};
