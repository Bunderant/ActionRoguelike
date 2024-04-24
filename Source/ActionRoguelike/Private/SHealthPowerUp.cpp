// Fill out your copyright notice in the Description page of Project Settings.

#include "SHealthPowerUp.h"

#include "SAttributeComponent.h"
#include "SCharacter.h"
#include "SPlayerState.h"

#define LOCTEXT_NAMESPACE "InteractableActors"

// Sets default values
ASHealthPowerUp::ASHealthPowerUp()
{
	HealthAmount = 10000.0f;
	CreditCost = 20;
	
	CooldownTime = 10.0f;
}

bool ASHealthPowerUp::CheckCanInteract(const APawn* InstigatorPawn)
{
	if (!Super::CheckCanInteract(InstigatorPawn))
		return false;
	
	if (!IsValid(InstigatorPawn) || !InstigatorPawn->IsA(ASCharacter::StaticClass()))
	{
		return false;
	}

	// Don't bother looking at "transactional" values yet. We'll let the player attempt to interact. This is just a
	// first-pass check for compatible instigator types. 
	return true;
}

FText ASHealthPowerUp::GetInteractText_Implementation(APawn* InstigatorPawn)
{
	USAttributeComponent* AttributeComp = USAttributeComponent::GetAttribute(InstigatorPawn);
	if (AttributeComp && AttributeComp->IsHealthFull())
	{
		return LOCTEXT("HealthPotion_FullHealthWarning", "Already at full health.");
	}

	return FText::Format(LOCTEXT("HealthPotion_InteractMessage", "Consume {0} Credits for full health."), CreditCost);
}

void ASHealthPowerUp::Apply(APawn* InstigatorPawn)
{
	ASPlayerState* PlayerState = InstigatorPawn->GetPlayerState<ASPlayerState>();
	if (!PlayerState)
		return;

	USAttributeComponent* HealthAttribute = InstigatorPawn->FindComponentByClass<USAttributeComponent>();
	if (!HealthAttribute)
		return;

	if (HealthAttribute->IsHealthFull())
	{
		return;
	}
	
	if (!PlayerState->DecrementCredits(CreditCost))
	{
		return;
	}

	HealthAttribute->ApplyHealthChange(InstigatorPawn, HealthAmount);
	
	Hide();
	StartCooldownTimer();
}

#undef LOCTEXT_NAMESPACE

