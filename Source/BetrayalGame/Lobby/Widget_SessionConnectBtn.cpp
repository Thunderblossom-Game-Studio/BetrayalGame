// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget_SessionConnectBtn.h"
#include "BetrayalGame/BetrayalGameInstance.h"


void UWidget_SessionConnectBtn::NativePreConstruct()
{
	Super::NativePreConstruct();

	_SessionName = FName(*_SessionData.SearchResult.Session.OwningUserName);
	_ConnectedPlayers = _SessionData.SearchResult.Session.SessionSettings.NumPublicConnections - _SessionData.SearchResult.Session.NumOpenPublicConnections;
	_MaxPlayers = _SessionData.SearchResult.Session.SessionSettings.NumPublicConnections;
	_Ping = _SessionData.SearchResult.PingInMs;
	_SearchResultsIndex = _SessionData.SessionIndex;

	// Assume if session has private connections, it's password protected
	if(_SessionData.SearchResult.Session.SessionSettings.NumPrivateConnections > 0)
	{
		_SessionName = FName(_SessionName.ToString() + " (P)");
		_bPrivate = true;
	}
}

bool UWidget_SessionConnectBtn::CheckPassword()
{
	const auto PasswordData = _SessionData.SearchResult.Session.SessionSettings.Settings.FindRef(PASSWORD).Data;
	FString Password;
	PasswordData.GetValue(Password);

	if (Password.IsEmpty())
	{
		// No password, just join
		return true;
	}
	else
	{
		// Check password against stored password
		const FString StoredPassword = GetGameInstance<UBetrayalGameInstance>()->SessionPassword;
		if (StoredPassword.IsEmpty())
		{
			// No stored password, prompt for password
			return false;
		}
		else
		{
			// Compare stored password with session password
			return StoredPassword == Password;
		}
	}
}
