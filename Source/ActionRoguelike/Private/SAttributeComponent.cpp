﻿// Fill out your copyright notice in the Description page of Project Settings.


#include "SAttributeComponent.h"

#include "SGameModeBase.h"
#include "Net/UnrealNetwork.h"


// Sets default values for this component's properties
USAttributeComponent::USAttributeComponent()
{
	Health = 100.0f;
	MaxHealth = 100.0f;
	
	Rage = 0.0f;
	MaxRage = 100.0f;

	SetIsReplicatedByDefault(true);
}

USAttributeComponent* USAttributeComponent::GetAttribute(const AActor* Actor)
{
	return Actor != nullptr
		? Actor->FindComponentByClass<USAttributeComponent>()
		: nullptr;
}

bool USAttributeComponent::IsActorAlive(const AActor* Actor)
{
	const USAttributeComponent* AttributeComp = GetAttribute(Actor);
	return AttributeComp ? AttributeComp->IsAlive() : false;
}

bool USAttributeComponent::ApplyHealthChange(AActor* InstigatorActor, const float Delta)
{
	if (Delta < 0.0f && !GetOwner()->CanBeDamaged())
	{
		return false;
	}
	
	const float PreviousHealth = Health;
	
	const float NewHealth = FMath::Clamp(Health + Delta, 0, MaxHealth);

	if (FMath::IsNearlyEqual(PreviousHealth,NewHealth))
	{
		return false;
	}

	const float ActualDelta = NewHealth - PreviousHealth;

	// Only apply the health change server-side, the the network will be notified via the multicast delegate
	if (GetOwner()->HasAuthority())
	{
		if (ActualDelta != 0.0f)
		{
			Health = NewHealth;
			MulticastHealthChanged(InstigatorActor, NewHealth, ActualDelta);

			if (ActualDelta < 0.0f)
			{
				ApplyRageChange(InstigatorActor, -ActualDelta);
			}
		}

		if (ActualDelta < 0.0f && !IsAlive())
		{
			if (ASGameModeBase* GM = GetWorld()->GetAuthGameMode<ASGameModeBase>(); IsValid(GM))
			{
				GM->OnActorKilled(GetOwner(), InstigatorActor);
			}
		}
	}

	// We've bailed early already if the prev value wasn't equal to the new, just return "true"
	return true;
}

bool USAttributeComponent::ApplyRageChange(AActor* InstigatorActor, float Delta)
{
	const int32 PreviousRage = Rage;
	const int32 NewRage = FMath::Clamp(Rage + Delta, 0, MaxRage);

	const int32 ActualDelta = NewRage - PreviousRage;
	if (ActualDelta == 0)
		return false;

	// Only update rage directly on server (replicated), then notify the network
	if (GetOwner()->HasAuthority())
	{
		Rage = NewRage;
		MulticastRageChanged(InstigatorActor, NewRage, ActualDelta);
	}

	// ActualDelta is non-zero, so return true
	return true;
}

bool USAttributeComponent::RecoverMaxHealth(AActor* InstigatorActor)
{
	return ApplyHealthChange(InstigatorActor, MaxHealth);
}

bool USAttributeComponent::Kill(AActor* InstigatorActor)
{
	return ApplyHealthChange(InstigatorActor, -MaxHealth);
}

bool USAttributeComponent::IsAlive() const
{
	return Health > 0.0f;
}

/**
 * @note Do not use from NetMulticast delegates when the value changes, since there is no guarantee the value itself is
 * up-to-date. 
 * @return Normalized Health
 */
float USAttributeComponent::GetHealthAsPercent() const
{
	if (FMath::IsNearlyZero(MaxHealth) || FMath::IsNearlyZero(Health))
	{
		return 0.0f;
	}

	return FMath::Clamp(Health / MaxHealth, 0.0f, 1.0f);
}

/**
 * @note Do not use from NetMulticast delegates when the value changes, since there is no guarantee the value itself is
 * up-to-date. 
 * @return Normalized Rage
 */
float USAttributeComponent::GetRageAsPercent() const
{
	if (!ensureAlwaysMsgf(MaxRage > 0, TEXT("MaxRage is set to 0, percentage will always be 0.")))
		return 0.0f;
	
	return FMath::Clamp(Rage / MaxRage, 0.0f, 1.0f);
}

void USAttributeComponent::MulticastHealthChanged_Implementation(AActor* InstigatorActor, float Value, float Delta)
{
	OnHealthChanged.Broadcast(InstigatorActor, this, Value, Delta);
}

void USAttributeComponent::MulticastRageChanged_Implementation(AActor* InstigatorActor, float Value, float Delta)
{
	OnRageChanged.Broadcast(InstigatorActor, this, Value, Delta);
}

void USAttributeComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(USAttributeComponent, Health);
	DOREPLIFETIME(USAttributeComponent, MaxHealth);

	DOREPLIFETIME(USAttributeComponent, Rage);
	DOREPLIFETIME(USAttributeComponent, MaxRage);
}

