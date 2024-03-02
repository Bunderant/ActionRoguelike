// Fill out your copyright notice in the Description page of Project Settings.


#include "SAttributeComponent.h"


// Sets default values for this component's properties
USAttributeComponent::USAttributeComponent()
{
	Health = 100.0f;
	MaxHealth = 100.0f;
}

bool USAttributeComponent::ApplyHealthChange(const float Delta)
{
	float PreviousValue = Health;
	
	Health = FMath::Clamp(Health + Delta, 0, MaxHealth);

	if (FMath::IsNearlyEqual(PreviousValue,Health))
	{
		return false;
	}

	OnAttributeChanged.Broadcast(nullptr, this, Health, Health - PreviousValue);
	return true;
}

bool USAttributeComponent::IsAlive() const
{
	return Health > 0.0f;
}

