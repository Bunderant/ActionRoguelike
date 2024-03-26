// Fill out your copyright notice in the Description page of Project Settings.


#include "SPlayerState.h"

ASPlayerState::ASPlayerState()
{
	NumCredits = 0;
}

void ASPlayerState::ClearNonPersistentState()
{
	UE_LOG(LogTemp, Warning, TEXT("Clearing non-persistent player state."));
}

bool ASPlayerState::IncrementCredits()
{
	if (NumCredits == MAX_int32)
	{
		return false;
	}
	
	NumCredits++;
	OnCreditsValueChanged.Broadcast(NumCredits, 1);
	return true;
}

bool ASPlayerState::DecrementCredits()
{
	if (NumCredits == 0)
	{
		return false;
	}
	
	NumCredits--;
	OnCreditsValueChanged.Broadcast(NumCredits, -1);
	return true;
}

int32 ASPlayerState::GetNumCredits() const
{
	return NumCredits;
}

