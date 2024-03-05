// Fill out your copyright notice in the Description page of Project Settings.

#include "VoiceChatSubsystem.h"
#include "BetrayalGameNetworkSubsystem.h"
#include "IOnlineSubsystemEOS.h"
#include "VoiceChat.h"
#include "OnlineSubsystem.h"
#include "GameFramework/PlayerState.h"

void UVoiceChatSubsystem::Init()
{
	LocalPlayerId = GetGameInstance()->GetFirstGamePlayer()->GetPreferredUniqueNetId().GetUniqueNetId();
	if(!LocalPlayerId.IsValid())
		UE_LOG(LogTemp, Error, TEXT("UVoiceChatSubsystem::Initialize() - LocalPlayerId is not valid!"));
	
	EOS = static_cast<IOnlineSubsystemEOS*>(IOnlineSubsystem::Get());
	if(!EOS)
		UE_LOG(LogTemp, Error, TEXT("UVoiceChatSubsystem::Initialize() - EOS is not valid!"));

	LocalVoiceChatUser = EOS->GetVoiceChatUserInterface(*LocalPlayerId);

	UE_LOG(LogTemp, Display, TEXT("UVoiceChatSubsystem::Initialize() - Initialized!"));
}

void UVoiceChatSubsystem::ChangeVolume(float NewVolume)
{
	if(!LocalVoiceChatUser)
	{
		UE_LOG(LogTemp, Error, TEXT("UVoiceChatSubsystem::ChangeVolume() - LocalVoiceChatUser is not valid!"));
		return;
	}

	LocalVoiceChatUser->SetAudioOutputVolume(NewVolume);
	Volume = NewVolume;
	UE_LOG(LogTemp, Display, TEXT("UVoiceChatSubsystem::ChangeVolume() - Volume changed to %f"), NewVolume);
}

void UVoiceChatSubsystem::ChangeUserVolume(APlayerState* Player, float NewVolume)
{
	if(!LocalVoiceChatUser)
	{
		UE_LOG(LogTemp, Error, TEXT("UVoiceChatSubsystem::ChangeUserVolume() - LocalVoiceChatUser is not valid!"));
		return;
	}

	LocalVoiceChatUser->SetPlayerVolume(Player->GetName(), NewVolume);
	UE_LOG(LogTemp, Display, TEXT("UVoiceChatSubsystem::ChangeUserVolume() - Volume changed to %f for user %s"), NewVolume, *UserId.ToString());
}
