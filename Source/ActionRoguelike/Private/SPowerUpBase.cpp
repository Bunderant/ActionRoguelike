// Fill out your copyright notice in the Description page of Project Settings.


#include "SPowerUpBase.h"


// Sets default values
ASPowerUpBase::ASPowerUpBase()
{
	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>("Static Mesh");
	RootComponent = StaticMesh;

	CooldownTime = 5.0f;

	bReplicates = true;
}

void ASPowerUpBase::Interact_Implementation(APawn* InstigatorPawn)
{
	if (!CheckCanInteract(InstigatorPawn))
	{
		return;
	}

	Apply(InstigatorPawn);
}

bool ASPowerUpBase::CheckCanInteract(const APawn* InstigatorPawn)
{
	if (CooldownTime > 0.0f && GetWorldTimerManager().IsTimerActive(CooldownTimerHandle))
	{
		return false;
	}
	
	return true;
}

void ASPowerUpBase::Apply(APawn* InstigatorPawn)
{
	unimplemented();
}

void ASPowerUpBase::Hide()
{
	SetActorHiddenInGame(true);
	SetActorEnableCollision(false);
}

void ASPowerUpBase::Show()
{
	SetActorHiddenInGame(false);
	SetActorEnableCollision(true);
}

void ASPowerUpBase::StartCooldownTimer()
{
	GetWorldTimerManager().SetTimer(CooldownTimerHandle, this, &ASPowerUpBase::FinishCooldown, CooldownTime);
}

void ASPowerUpBase::FinishCooldown()
{
	Show();
}
