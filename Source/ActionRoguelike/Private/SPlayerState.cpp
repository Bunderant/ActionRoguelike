// Fill out your copyright notice in the Description page of Project Settings.


#include "SPlayerState.h"

#include "SAttributeComponent.h"

ASPlayerState::ASPlayerState()
{
	NumCredits = 0;
	RageAmount = 0;
	MaxRage = 50.0f;
}

void ASPlayerState::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	
	OnPawnSet.AddDynamic(this, &ASPlayerState::HandlePawnSet);
}

void ASPlayerState::ClearNonPersistentState()
{
	SetRageAmount(0);
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

bool ASPlayerState::IncrementRage(int32 Amount)
{
	if (!ensureAlways(Amount > 0) || RageAmount == MaxRage)
		return false;

	SetRageAmount(RageAmount + Amount);

	FString DebugMsg = FString::Printf(TEXT("Incremented Rage. New Value: %d"), RageAmount);
	GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Magenta, DebugMsg);

	return true;
}

bool ASPlayerState::DecrementRage(int32 Amount)
{
	if (!ensureAlways(Amount > 0) || RageAmount < Amount)
		return false;

	SetRageAmount(RageAmount - Amount);

	return true;
}

float ASPlayerState::GetRagePercent() const
{
	if (!ensureAlwaysMsgf(MaxRage > 0, TEXT("MaxRage is set to 0, percentage will always be 0.")))
		return 0.0f;
	
	return RageAmount / static_cast<float>(MaxRage);
}

void ASPlayerState::SetRageAmount(const int32 Value)
{
	const int32 PreviousRage = RageAmount;
	RageAmount = FMath::Clamp(Value, 0, MaxRage);

	if (RageAmount != PreviousRage)
	{
		OnRageValueChanged.Broadcast(RageAmount, RageAmount - PreviousRage);
	}
}

void ASPlayerState::HandlePawnSet(APlayerState* Player, APawn* NewPawn, APawn* OldPawn)
{
	// When a new pawn is set, listen for any changes that we may need to update the player state
	if (USAttributeComponent* AttributeComp = USAttributeComponent::GetAttribute(NewPawn))
	{
		UE_LOG(LogTemp, Warning, TEXT("Player state is listening for player health changes."));
		AttributeComp->OnAttributeChanged.AddDynamic(this, &ASPlayerState::OnPlayerHealthChanged);
	}
}

void ASPlayerState::OnPlayerHealthChanged(AActor* InstigatorActor, USAttributeComponent* OwningComponent, float Value,
	float Delta)
{
	if (Delta < 0.0f)
	{
		IncrementRage(static_cast<int32>(-Delta));
	}
}

