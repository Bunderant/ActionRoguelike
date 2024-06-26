﻿// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SAction.h"
#include "SActionEffect.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONROGUELIKE_API USActionEffect : public USAction
{
	GENERATED_BODY()

public:
	USActionEffect();
	
	virtual void StartAction_Implementation(AActor* Instigator) override;
	virtual void StopAction_Implementation(AActor* Instigator) override;

	UFUNCTION(BlueprintPure, Category="Effect")
	float GetRemainingTime() const;

	UFUNCTION(BlueprintPure, Category="Effect")
	float GetRemainingTimeNormalized() const;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Effect")
	float Duration;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Effect")
	float Period;

	UFUNCTION(BlueprintNativeEvent, Category="Effect")
	void OnPeriodElapsed(AActor* Instigator);

	FTimerHandle TimerHandle_Duration;
	FTimerHandle TimerHandle_Period;
};
