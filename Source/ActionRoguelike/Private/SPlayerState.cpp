// Fill out your copyright notice in the Description page of Project Settings.


#include "SPlayerState.h"

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
