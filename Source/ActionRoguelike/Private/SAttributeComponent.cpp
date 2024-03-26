// Fill out your copyright notice in the Description page of Project Settings.


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
	
	return true;
}

bool USAttributeComponent::ApplyRageChange(AActor* InstigatorActor, float Delta)
{
	const int32 PreviousRage = Rage;
	Rage = FMath::Clamp(Rage + Delta, 0, MaxRage);

	if (Rage != PreviousRage)
	{
		MulticastRageChanged(InstigatorActor, Rage, Rage - PreviousRage);
		return true;
	}

	return false;
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

float USAttributeComponent::GetHealthAsPercent() const
{
	if (FMath::IsNearlyZero(MaxHealth) || FMath::IsNearlyZero(Health))
	{
		return 0.0f;
	}

	return FMath::Clamp(Health / MaxHealth, 0.0f, 1.0f);
}

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

