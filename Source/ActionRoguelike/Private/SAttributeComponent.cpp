// Fill out your copyright notice in the Description page of Project Settings.


#include "SAttributeComponent.h"


// Sets default values for this component's properties
USAttributeComponent::USAttributeComponent()
{
	Health = 100.0f;
	MaxHealth = 100.0f;
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
	const float PreviousValue = Health;
	
	Health = FMath::Clamp(Health + Delta, 0, MaxHealth);

	if (FMath::IsNearlyEqual(PreviousValue,Health))
	{
		return false;
	}

	OnAttributeChanged.Broadcast(InstigatorActor, this, Health, Health - PreviousValue);
	return true;
}

bool USAttributeComponent::RecoverMaxHealth(AActor* InstigatorActor)
{
	return ApplyHealthChange(InstigatorActor, MaxHealth);
}

bool USAttributeComponent::IsAlive() const
{
	return Health > 0.0f;
}

bool USAttributeComponent::IsFull() const
{
	return FMath::IsNearlyEqual(Health, MaxHealth);
}

float USAttributeComponent::GetHealthAsPercent() const
{
	if (FMath::IsNearlyZero(MaxHealth) || FMath::IsNearlyZero(Health))
	{
		return 0.0f;
	}

	return Health / MaxHealth;
}

