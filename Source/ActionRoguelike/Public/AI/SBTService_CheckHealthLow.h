// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "SBTService_CheckHealthLow.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONROGUELIKE_API USBTService_CheckHealthLow : public UBTService
{
	GENERATED_BODY()

protected:

	UPROPERTY(EditAnywhere, Category="AI")
	FBlackboardKeySelector IsHealthLowKey;

	UPROPERTY(EditAnywhere, Category="AI")
	float LowHealthThresholdPercent;

	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
};
