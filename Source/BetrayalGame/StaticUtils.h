#pragma once

#include "CoreMinimal.h"

inline void Print(const FString& Text) {
	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, Text);
}