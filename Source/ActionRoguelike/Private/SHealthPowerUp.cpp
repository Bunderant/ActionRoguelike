// Fill out your copyright notice in the Description page of Project Settings.

#include "SHealthPowerUp.h"

#include "SAttributeComponent.h"
#include "SCharacter.h"
#include "SPlayerState.h"


// Sets default values
ASHealthPowerUp::ASHealthPowerUp()
{
	HealthAmount = 10000.0f;
	CooldownTime = 10.0f;
}

bool ASHealthPowerUp::CheckCanInteract(const APawn* InstigatorPawn)
{
	if (GetWorldTimerManager().IsTimerActive(CooldownTimerHandle) ||
		!InstigatorPawn ||
		!InstigatorPawn->IsA(ASCharacter::StaticClass()))
	{
		return false;
	}

	const USAttributeComponent* HealthAttribute = InstigatorPawn->FindComponentByClass<USAttributeComponent>();
	if (!HealthAttribute)
	{
		return false;
	}

	return !HealthAttribute->IsHealthFull();
}

void ASHealthPowerUp::Apply(APawn* InstigatorPawn)
{
	ASPlayerState* PlayerState = InstigatorPawn->GetPlayerState<ASPlayerState>();
	if (!PlayerState || !PlayerState->DecrementCredits())
	{
		return;
	}
	
	GetWorldTimerManager().ClearTimer(CooldownTimerHandle);
	GetWorldTimerManager().SetTimer(CooldownTimerHandle, this, &ASHealthPowerUp::FinishCooldown, CooldownTime);

	Hide();
	
	USAttributeComponent* HealthAttribute = InstigatorPawn->FindComponentByClass<USAttributeComponent>();
	HealthAttribute->ApplyHealthChange(this, HealthAmount);
}

void ASHealthPowerUp::FinishCooldown()
{
	Show();
}

