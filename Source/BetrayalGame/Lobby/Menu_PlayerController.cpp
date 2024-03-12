// Fill out your copyright notice in the Description page of Project Settings.

#include "Menu_PlayerController.h"
#include "../StaticUtils.h"
#include "OnlineSubsystem.h"
#include "OnlineSubsystemUtils.h"
#include "OnlineSubsystemTypes.h"
#include "VoiceChatSubsystem.h"
#include "Interfaces/OnlineIdentityInterface.h"

AMenu_PlayerController::AMenu_PlayerController()
{
}

void AMenu_PlayerController::BeginPlay()
{
	Super::BeginPlay();
	Login();
}

void AMenu_PlayerController::Login()
{
	auto OnlineSubsystem = IOnlineSubsystem::Get();
	if (!OnlineSubsystem)
	{
		Print("No OnlineSubsystem found!");
		return;
	}

	auto Identity = OnlineSubsystem->GetIdentityInterface();
	if (!Identity.IsValid())
	{
		Print("No IdentityInterface found!");
		return;
	}

	
	FUniqueNetIdPtr UserId = Identity->GetUniquePlayerId(0);
	if (UserId && Identity->GetLoginStatus(0) == ELoginStatus::LoggedIn)
	{
		Print("Already logged in!");
		auto User = Identity->GetUserAccount(*UserId);
		if(User.IsValid())
		{
			Print("User: " + User->GetDisplayName());
		}
	}

	LoginDelegateHandle = Identity->AddOnLoginCompleteDelegate_Handle(0,
	                                                                  FOnLoginCompleteDelegate::CreateUObject(
		                                                                  this,
		                                                                  &AMenu_PlayerController::OnLoginCompleted));

	FString AuthType;
	FParse::Value(FCommandLine::Get(), TEXT("AUTH_TYPE="), AuthType);

	if (!AuthType.IsEmpty())
	{
		Print("Logging in with AuthType: " + AuthType);
		if (!Identity->AutoLogin(0))
		{
			Print("Failed to login with AuthType: " + AuthType);
			Identity->ClearOnLoginCompleteDelegate_Handle(0, LoginDelegateHandle);
			LoginDelegateHandle.Reset();
		}
	}
	else
	{
	    FOnlineAccountCredentials Credentials("AccountPortal", "", "");

		Print("Logging in with default AuthType");

		if(!Identity->Login(0, Credentials))
		{
			Print("Failed to login with default AuthType");
			Identity->ClearOnLoginCompleteDelegate_Handle(0, LoginDelegateHandle);
			LoginDelegateHandle.Reset();
		}
	}
}

void AMenu_PlayerController::OnLoginCompleted(int32 LocalUserNum, bool bWasSuccessful, const FUniqueNetId& UserId,
	const FString& Error)
{
	auto Subsystem = IOnlineSubsystem::Get();
	auto Identity = Subsystem->GetIdentityInterface();

	if (!Subsystem || !Identity)
		return;

	if(bWasSuccessful)
	{
		Print("Login successful!");
		auto User = Identity->GetUserAccount(UserId);
		if(User.IsValid())
		{
			Print("User: " + User->GetDisplayName());
		}
	}
	else
	{
		Print("Login failed: " + Error);	
	}

	Identity->ClearOnLoginCompleteDelegate_Handle(LocalUserNum, LoginDelegateHandle);
	LoginDelegateHandle.Reset();

	// Set up the voice chat subsystem
	auto VC = GetGameInstance()->GetSubsystem<UVoiceChatSubsystem>();
	if(VC)
		VC->Init();
}
