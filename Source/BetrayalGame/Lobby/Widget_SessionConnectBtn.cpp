// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget_SessionConnectBtn.h"

#include "BetrayalGameNetworkSubsystem.h"
#include "BetrayalGame/BetrayalGameInstance.h"
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
		
		// No password, just join
		return true;
	}

	// Check password against stored password
	const FString StoredPassword = GetGameInstance<UBetrayalGameInstance>()->SessionPassword;
	if (StoredPassword.IsEmpty())
	{
		// No stored password, prompt for password
		return false;
	}

	// Compare stored password with session password
	return StoredPassword == Password;
}

void UWidget_SessionConnectBtn::NativeConstruct()
{
	Super::NativeConstruct();

	// Bind OnClick to the button
	if (UButton* Button = Cast<UButton>(GetWidgetFromName("Btn_Join")))
	{
		Button->OnClicked.AddDynamic(this, &UWidget_SessionConnectBtn::OnClick);
	}
}

void UWidget_SessionConnectBtn::OnClick()
{
	// Check if the session is password protected
	if (CheckPassword())
	{
		// Join the session
		GetGameInstance<UBetrayalGameInstance>()->JoinSession(GetOwningLocalPlayer(), _SessionData);
	}
	else
	{
		// Prompt for password
		GetGameInstance<UBetrayalGameInstance>()->ShowPasswordField();

		// Bind OnClick to the password field to join the session
		if (UButton* Button = Cast<UButton>(GetGameInstance<UBetrayalGameInstance>()->WB_PasswordField->GetWidgetFromName("Btn_AcceptPswd")))
		{
			Button->OnClicked.AddDynamic(this, &UWidget_SessionConnectBtn::OnClick);
		}
	}
}
