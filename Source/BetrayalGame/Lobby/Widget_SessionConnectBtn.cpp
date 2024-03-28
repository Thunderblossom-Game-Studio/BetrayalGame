// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget_SessionConnectBtn.h"

#include "BetrayalGameNetworkSubsystem.h"
#include "Menu_PlayerController.h"
#include "BetrayalGame/BetrayalGameInstance.h"
#include "BetrayalGame/StaticUtils.h"
#include "Components/Button.h"

bool UWidget_SessionConnectBtn::CheckPassword()
{
	const auto PasswordData = _SessionData.Session.SessionSettings.Settings.FindRef(PASSWORD).Data;
	FString Password;
	PasswordData.GetValue(Password);

	if (Password.IsEmpty())
	{
		// Hide the password field
		APlayerController* Ctrl = GetGameInstance()->GetFirstLocalPlayerController();
		if(auto Menu = Cast<AMenu_PlayerController>(Ctrl))
		{
			Menu->HidePasswordField();
		}
			
		PrintLog("No password required!");
		
		// No password, just join
		return true;
	}

	// Check password against stored password
	//const FString StoredPassword = GetGameInstance<UBetrayalGameInstance>()->SessionPassword;
	APlayerController* Ctrl = GetGameInstance()->GetFirstLocalPlayerController();
	auto Menu = Cast<AMenu_PlayerController>(Ctrl);
	if(!Menu)
	{
		PrintLog("UWidget_SessionConnectBtn::CheckPassword(): Menu is null!");
		return false;
	}

	if (Menu->SessionPassword.IsEmpty())
	{
		PrintLog("Password input required!");
		
		// No stored password, prompt for password
		return false;
	}

	PrintLog("Checking password...");
	// Compare stored password with session password
	return Menu->SessionPassword == Password;
}

void UWidget_SessionConnectBtn::NativeConstruct()
{
	Super::NativeConstruct();

	// Bind OnClick to the button
	if (UButton* Button = Cast<UButton>(GetWidgetFromName("Btn_JoinSession")))
	{
		Button->OnClicked.AddDynamic(this, &UWidget_SessionConnectBtn::OnClick);
	}
	else
	{
		PrintLog("UWidget_SessionConnectBtn::NativeConstruct(): Button is null!");
	}
}

void UWidget_SessionConnectBtn::OnClick()
{
	PrintLog("Joining session: " + _SessionData.Session.OwningUserName);

	//GetGameInstance<UBetrayalGameInstance>()->HideLobby();
	APlayerController* Ctrl = GetGameInstance()->GetFirstLocalPlayerController();
	auto Menu = Cast<AMenu_PlayerController>(Ctrl);
	if(!Menu)
	{
		PrintLog("UWidget_SessionConnectBtn::OnClick(): Menu is null!");
		return;
	}
	
		Menu->HideLobby();
	// Check if the session is password protected
	if (CheckPassword())
	{
		PrintLog("Joining session!");
		// Join the session
		GetGameInstance<UBetrayalGameInstance>()->GetSubsystem<UBetrayalGameNetworkSubsystem>()->JoinSession(_SessionName, _SessionData);
	}
	else
	{
		PrintLog("UWidget_SessionConnectBtn::OnClick(): Password required!");

		// Prompt for password
		//GetGameInstance<UBetrayalGameInstance>()->ShowPasswordField();
		if(Menu)
		{
			Menu->ShowPasswordField();
		}

		
		// Bind OnClick to the password field to join the session
		if (UButton* Button = Cast<UButton>(Menu->WB_PasswordField->GetWidgetFromName("Btn_AcceptPswd")))
		{
			// Clear other bindings from the button
			Button->OnClicked.Clear();
			// Bind OnClick to this widget instance's OnClick
			Button->OnClicked.AddDynamic(this, &UWidget_SessionConnectBtn::OnClick);
		}
	}
}

void UWidget_SessionConnectBtn::UpdateDisplayedData()
{
	PrintLog("Updating data!");
	
	// Set the displayed data
	const auto ListedName = _SessionData.Session.SessionSettings.Settings.FindRef(SERVERLIST_NAME).Data;
	FString Name = "";
	ListedName.GetValue(Name);
	_SessionName = FName(*Name); 

	// Connected players
	_ConnectedPlayers += _SessionData.Session.SessionSettings.NumPublicConnections - _SessionData.Session.NumOpenPublicConnections;
	_ConnectedPlayers += _SessionData.Session.SessionSettings.NumPrivateConnections - _SessionData.Session.NumOpenPrivateConnections;

	// Total player slots
	_MaxPlayers += _SessionData.Session.SessionSettings.NumPublicConnections;
	_MaxPlayers += _SessionData.Session.SessionSettings.NumPrivateConnections;
	
	// Ping
	_Ping = _SessionData.PingInMs;

	OnDataUpdated();
}
