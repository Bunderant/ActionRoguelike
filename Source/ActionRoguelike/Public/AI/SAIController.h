// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIModule/Classes/AIController.h"
#include "SAIController.generated.h"

UCLASS()
class ACTIONROGUELIKE_API ASAIController : public AAIController
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ASAIController();

protected:

	UPROPERTY(EditDefaultsOnly, Category="AI")
	TObjectPtr<UBehaviorTree> BehaviorTree;
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
};
