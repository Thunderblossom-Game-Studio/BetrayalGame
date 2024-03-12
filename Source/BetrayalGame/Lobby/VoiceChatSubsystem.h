// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "IOnlineSubsystemEOS.h"
#include "Subsystems/GameInstanceSubsystem.h"

#include "VoiceChatSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class BETRAYALGAME_API UVoiceChatSubsystem : public UGameInstanceSubsystem
{
private:
	GENERATED_BODY()

	// Pointers to commonly used classes
	FUniqueNetIdPtr LocalPlayerId;
	class IOnlineSubsystemEOS* EOS;

	IVoiceChatUser* LocalVoiceChatUser;
	
	// Internal state
	UPROPERTY()
	bool bIsInitialized = false;

	UPROPERTY()
	bool bVoiceChatEnabled = false;
	
	UPROPERTY()
	float Volume = 1.0f;

	UPROPERTY()
	bool bIsMuted = false;

	UPROPERTY()
	bool bPushToTalk = false;

public:
	UFUNCTION(BlueprintCallable, Category = "VoiceChat")
	void Init();

	UFUNCTION(BlueprintCallable, Category = "VoiceChat")
	void ChangeVolume(float NewVolume);

	UFUNCTION(BlueprintCallable, Category = "VoiceChat")
	float GetVolume() const { return Volume; }
	
	UFUNCTION(Category = "VoiceChat")
	void ChangeUserVolume(APlayerState* Player, float NewVolume);
	
	// Toggle Mic Active
	UFUNCTION(BlueprintCallable, Category = "Networking")
	void StartPTT();

	UFUNCTION(BlueprintCallable, Category = "Networking")
	void StopPTT();
};
