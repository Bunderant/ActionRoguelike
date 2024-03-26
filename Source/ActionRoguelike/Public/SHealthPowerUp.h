// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SPowerUpBase.h"
#include "SHealthPowerUp.generated.h"

UCLASS()
class ACTIONROGUELIKE_API ASHealthPowerUp : public ASPowerUpBase
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ASHealthPowerUp();

protected:

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float HealthAmount;

	virtual bool CheckCanInteract(const APawn* InstigatorPawn) override;
	virtual void Apply(APawn* InstigatorPawn) override;
};
