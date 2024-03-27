// Fill out your copyright notice in the Description page of Project Settings.


#include "SPowerUpBase.h"

#include "ActionRoguelike/ActionRoguelike.h"
#include "Net/UnrealNetwork.h"


// Sets default values
ASPowerUpBase::ASPowerUpBase()
{
	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>("Static Mesh");
	RootComponent = StaticMesh;

	CooldownTime = 5.0f;

	bReplicates = true;
	bIsVisible = !StaticMesh->bHiddenInGame;
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
	ServerHide();
}

void ASPowerUpBase::Show()
{
	ServerShow();
}

void ASPowerUpBase::ServerShow_Implementation()
{
	if (bIsVisible)
		return;
	
	bIsVisible = true;
	OnRep_IsVisible();
}

void ASPowerUpBase::ServerHide_Implementation()
{
	if (!bIsVisible)
		return;
	
	bIsVisible = false;
	OnRep_IsVisible();
}

void ASPowerUpBase::StartCooldownTimer_Implementation()
{
	GetWorldTimerManager().SetTimer(CooldownTimerHandle, this, &ASPowerUpBase::FinishCooldown, CooldownTime);
}

void ASPowerUpBase::FinishCooldown()
{
	Show();
}

void ASPowerUpBase::OnRep_IsVisible()
{
	StaticMesh->SetHiddenInGame(!bIsVisible);
	StaticMesh->SetCollisionEnabled(bIsVisible ? ECollisionEnabled::QueryOnly : ECollisionEnabled::NoCollision);

	// @fixme: Strange things are afoot. Client/server don't rep in both places with this "typical" setup
	// SetActorHiddenInGame(!bIsVisible);
	// SetActorEnableCollision(bIsVisible);

	LogToScreen(this, FString::Printf(TEXT("Visibility Changed: %s"), bIsVisible ? TEXT("true") : TEXT("false")), FColor::Red);
}

void ASPowerUpBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASPowerUpBase, bIsVisible);
}
