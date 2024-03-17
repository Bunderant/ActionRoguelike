// Fill out your copyright notice in the Description page of Project Settings.


#include "SAttributeComponent.h"

#include "SGameModeBase.h"
#include "Net/UnrealNetwork.h"


// Sets default values for this component's properties
USAttributeComponent::USAttributeComponent()
{
	Health = 100.0f;
	MaxHealth = 100.0f;

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
	
	const float PreviousValue = Health;
	
	Health = FMath::Clamp(Health + Delta, 0, MaxHealth);

	if (FMath::IsNearlyEqual(PreviousValue,Health))
	{
		return false;
	}

	const float ActualDelta = Health - PreviousValue;

	if (ActualDelta != 0.0f)
	{
		MulticastHealthChanged(InstigatorActor, Health, ActualDelta);
	}

	if (ActualDelta < 0.0f && !IsAlive())
	{
		if (ASGameModeBase* GM = GetWorld()->GetAuthGameMode<ASGameModeBase>(); IsValid(GM))
		{
			GM->OnActorKilled(GetOwner(), InstigatorActor);
		}
	}
	
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

void USAttributeComponent::MulticastHealthChanged_Implementation(AActor* InstigatorActor, float Value, float Delta)
{
	OnAttributeChanged.Broadcast(InstigatorActor, this, Health, Delta);
}

void USAttributeComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(USAttributeComponent, Health);
	DOREPLIFETIME(USAttributeComponent, MaxHealth);
}

