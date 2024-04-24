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

bool ASPlayerState::IncrementCredits(const int32 Amount)
{
	if (!ensureAlways(Amount > 0))
	{
		return false;
	}
	
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

