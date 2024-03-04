// Fill out your copyright notice in the Description page of Project Settings.

#include "SHealthPowerUp.h"

#include "SAttributeComponent.h"
#include "SCharacter.h"


// Sets default values
ASHealthPowerUp::ASHealthPowerUp()
{
	HealthAmount = 10000.0f;
	CooldownTime = 10.0f;
}

bool ASHealthPowerUp::CheckCanInteract(const APawn* InstigatorPawn)
{
	return (
		!GetWorldTimerManager().IsTimerActive(CooldownTimerHandle) &&
		InstigatorPawn &&
		InstigatorPawn->IsA(ASCharacter::StaticClass())
	);
}

void ASHealthPowerUp::Apply(APawn* InstigatorPawn)
{
	GetWorldTimerManager().ClearTimer(CooldownTimerHandle);
	GetWorldTimerManager().SetTimer(CooldownTimerHandle, this, &ASHealthPowerUp::FinishCooldown, CooldownTime);

	SetActorHiddenInGame(true);
	SetActorEnableCollision(false);
	
	USAttributeComponent* HealthAttribute = Cast<USAttributeComponent>(InstigatorPawn->GetComponentByClass(USAttributeComponent::StaticClass()));
	HealthAttribute->ApplyHealthChange(HealthAmount);
}

void ASHealthPowerUp::FinishCooldown()
{
	SetActorHiddenInGame(false);
	SetActorEnableCollision(true);
}

