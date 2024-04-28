// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

DECLARE_STATS_GROUP(TEXT("STANFORD_Game"), STATGROUP_STANFORD, STATCAT_Advanced);

inline void LogToScreen(const UObject* WorldContext, const FString& DebugMsg, const FColor Color = FColor::White, const float Duration = 5.0f)
{
	if (!ensureMsgf(!DebugMsg.IsEmpty(), TEXT("Attempting to log empty string. Watchoo doing.")))
		return;
	
	if (!GEngine || !IsValid(WorldContext))
		return;

	const UWorld* World = WorldContext->GetWorld();
	if (!IsValid(World))
		return;

	const FString NetPrefix = World->IsNetMode(NM_Client) ? "[CLIENT] " : "[SERVER] ";

	GEngine->AddOnScreenDebugMessage(-1, Duration, Color, NetPrefix + DebugMsg);
}