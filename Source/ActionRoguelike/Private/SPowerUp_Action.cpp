// Fill out your copyright notice in the Description page of Project Settings.


#include "SPowerUp_Action.h"

#include "Actions/SAction.h"
#include "Actions/SActionComponent.h"


// Sets default values
ASPowerUp_Action::ASPowerUp_Action()
{ }

bool ASPowerUp_Action::CheckCanInteract(const APawn* InstigatorPawn)
{
	if (!ensure(ActionClass))
		return false;
	
	if (!IsValid(InstigatorPawn))
		return false;
	
	const USActionComponent* ActionComp = InstigatorPawn->FindComponentByClass<USActionComponent>();
	if (ActionComp == nullptr)
		return false;
	
	return !ActionComp->CheckHasAction(ActionClass);
}

void ASPowerUp_Action::Apply(APawn* InstigatorPawn)
{
	// If we we allowed to "apply", then at this point the action component has to exist
	USActionComponent* ActionComp = InstigatorPawn->FindComponentByClass<USActionComponent>();

	ActionComp->AddAction(InstigatorPawn, ActionClass);

	Hide();
}

