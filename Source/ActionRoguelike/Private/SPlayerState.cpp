// Fill out your copyright notice in the Description page of Project Settings.


#include "SPlayerState.h"
#include "SaveGame/SSaveGame.h"

#include "Net/UnrealNetwork.h"

ASPlayerState::ASPlayerState()
{
	NumCredits = 0;

	bReplicates = true;
}

void ASPlayerState::ClearNonPersistentState()
{
	UE_LOG(LogTemp, Log, TEXT("Clearing non-persistent player state."));
}

bool ASPlayerState::IncrementCredits(const int32 Amount)
{
	if (!ensureAlways(Amount > 0))
	{
		return false;
	}

	// @fixme: access violation reported here. I think it was due to a dying enemy incrementing credits on an already-dead player
	if (NumCredits == MAX_int32)
	{
		return false;
	}

	const int32 PrevNumCredits = NumCredits;
	NumCredits = FMath::Clamp(NumCredits + Amount, 0, MAX_int32);

	if (NumCredits != PrevNumCredits)
	{
		MulticastCreditsChanged(NumCredits, NumCredits - PrevNumCredits);
		return true;
	}
	
	return false;
}

bool ASPlayerState::DecrementCredits(const int32 Amount)
{
	if (!ensureAlways(Amount > 0))
	{
		return false;
	}
	
	if (NumCredits < Amount)
	{
		return false;
	}

	const int32 PrevNumCredits = NumCredits;
	NumCredits = FMath::Clamp(NumCredits - Amount, 0, MAX_int32);
	
	if (NumCredits != PrevNumCredits)
	{
		MulticastCreditsChanged(NumCredits, NumCredits - PrevNumCredits);
		return true;
	}
	
	return false;
}

bool ASPlayerState::ApplyCredits(int32 Delta)
{
	if (!ensureAlways(Delta != 0))
	{
		return false;
	}

	if (Delta > 0)
	{
		return IncrementCredits(Delta);
	}

	return DecrementCredits(-Delta);
}

void ASPlayerState::MulticastCreditsChanged_Implementation(float Value, float Delta)
{
	OnCreditsValueChanged.Broadcast(Value, Delta);
}

int32 ASPlayerState::GetNumCredits() const
{
	return NumCredits;
}

void ASPlayerState::WriteToSavedGame_Implementation(USSaveGame* SaveGameObject) const
{
	if (!ensureAlways(IsValid(SaveGameObject)))
		return;
	
	// Gather all relevant data for player
	FPlayerSaveData SaveData;
	SaveData.Credits = NumCredits;

	// Stored as FString for simplicity (original Steam ID is uint64)
	SaveData.PlayerID = GetUniqueId().ToString();

	// May not be alive while we save
	if (APawn* MyPawn = GetPawn())
	{
		SaveData.Location = MyPawn->GetActorLocation();
		SaveData.Rotation = MyPawn->GetActorRotation();
		SaveData.bUseSavedTransform = true;
	}
		
	SaveGameObject->SavedPlayers.Add(SaveData);
}

void ASPlayerState::LoadFromSavedGame_Implementation(USSaveGame* SaveObject)
{
	if (!ensureAlways(IsValid(SaveObject)))
		return;
	
	FPlayerSaveData* FoundData = SaveObject->GetPlayerData(this);
	if (FoundData)
	{
		// Makes sure we trigger credits changed event
		NumCredits = 0;
		IncrementCredits(FoundData->Credits);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Could not find SaveGame data for player id '%i'."), GetPlayerId());
	}
}

void ASPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASPlayerState, NumCredits);
}

