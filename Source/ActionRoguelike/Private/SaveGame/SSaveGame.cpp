// Fill out your copyright notice in the Description page of Project Settings.


#include "SaveGame/SSaveGame.h"

#include "ActionRoguelike/ActionRoguelike.h"
#include "GameFramework/PlayerState.h"
#include "Logging/StructuredLog.h"

FPlayerSaveData* USSaveGame::GetPlayerData(APlayerState* PlayerState)
{
	if (PlayerState == nullptr)
		return nullptr;

	if (PlayerState->GetWorld()->IsPlayInEditor())
	{
		UE_LOGFMT(LogGame, Log, "During PIE we cannnot use PlayerId to retreive saved player data. Usint 1st array entry, if present.");

		if (SavedPlayers.IsValidIndex(0))
		{
			return &SavedPlayers[0];
		}

		// No saved player data available
		return nullptr;
	}

	// Easiest way to deal with the different IDs is as FString (original Steam id is uint64)
	// Keep in mind that GetUniqueId() returns the online id, where GetUniqueID() is a function from UObject (very confusing...)
	const FString PlayerID = PlayerState->GetUniqueId().ToString();
	// Iterate the array and match by PlayerID (e.g. unique ID provided by Steam)
	return SavedPlayers.FindByPredicate([&](const FPlayerSaveData& Data) { return Data.PlayerID == PlayerID; });
}
