// Fill out your copyright notice in the Description page of Project Settings.


#include "SPlayerState.h"

#include "Net/UnrealNetwork.h"

ASPlayerState::ASPlayerState()
{
	NumCredits = 0;

	bReplicates = true;
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
	MulticastCreditsChanged(NumCredits, 1);
	return true;
}

bool ASPlayerState::DecrementCredits()
{
	if (NumCredits == 0)
	{
		return false;
	}
	
	NumCredits--;
	MulticastCreditsChanged(NumCredits, -1);
	return true;
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
	
	SaveGameObject->Credits = NumCredits;
}

void ASPlayerState::LoadFromSavedGame_Implementation(const USSaveGame* SaveGameObject)
{
	if (!ensureAlways(IsValid(SaveGameObject)))
		return;
	
	NumCredits = SaveGameObject->Credits;
}

void ASPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASPlayerState, NumCredits);
}

