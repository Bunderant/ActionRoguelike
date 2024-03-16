// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SPowerUpBase.h"
#include "SPowerUp_Action.generated.h"

class USAction;

UCLASS()
class ACTIONROGUELIKE_API ASPowerUp_Action : public ASPowerUpBase
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ASPowerUp_Action();

protected:
	UPROPERTY(EditDefaultsOnly, Category="Action")
	TSubclassOf<USAction> ActionClass;
	
	virtual bool CheckCanInteract(const APawn* InstigatorPawn) override;

	virtual void Apply(APawn* InstigatorPawn) override;
};
