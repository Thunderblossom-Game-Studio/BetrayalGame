#pragma once

#include "CoreMinimal.h"

inline void PrintWarning(const FString& Text) {
	UE_LOG(LogTemp, Warning, TEXT("%s"), *Text);
}

inline void PrintError(const FString& Text)
{
	UE_LOG(LogTemp, Error, TEXT("%s"), *Text);
}

inline void PrintLog(const FString& Text)
{
	UE_LOG(LogTemp, Display, TEXT("%s"), *Text);
}
