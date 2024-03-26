// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SPowerUpBase.h"
#include "SCoinPowerUp.generated.h"

UCLASS()
class ACTIONROGUELIKE_API ASCoinPowerUp : public ASPowerUpBase
{
	GENERATED_BODY()

protected:
	virtual void Apply(APawn* InstigatorPawn) override;
};
