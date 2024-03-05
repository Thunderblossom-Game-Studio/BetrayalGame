// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget_SessionConnectBtn.h"

#include "BetrayalGameNetworkSubsystem.h"
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
		GetGameInstance<UBetrayalGameInstance>()->HidePasswordField();

		Print("No password required!");
		
		// No password, just join
		return true;
	}

	// Check password against stored password
	const FString StoredPassword = GetGameInstance<UBetrayalGameInstance>()->SessionPassword;
	if (StoredPassword.IsEmpty())
	{
		Print("Password input required!");
		
		// No stored password, prompt for password
		return false;
	}

	Print("Checking password...");
	// Compare stored password with session password
	return StoredPassword == Password;
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
		Print("UWidget_SessionConnectBtn::NativeConstruct(): Button is null!");
	}
}

void UWidget_SessionConnectBtn::OnClick()
{
	Print("Joining session: " + _SessionData.Session.OwningUserName);
	
	// Check if the session is password protected
	if (CheckPassword())
	{
		Print("Joining session!");
		// Join the session
		GetGameInstance<UBetrayalGameInstance>()->GetSubsystem<UBetrayalGameNetworkSubsystem>()->JoinSession(_SessionName, _SessionData);
	}
	else
	{
		Print("UWidget_SessionConnectBtn::OnClick(): Password required!");

		// Prompt for password
		GetGameInstance<UBetrayalGameInstance>()->ShowPasswordField();

		// Bind OnClick to the password field to join the session
		if (UButton* Button = Cast<UButton>(GetGameInstance<UBetrayalGameInstance>()->WB_PasswordField->GetWidgetFromName("Btn_AcceptPswd")))
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
	Print("Updating data!");
	
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
