﻿// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SAICharacter.generated.h"

class USAttributeComponent;
class UPawnSensingComponent;

UCLASS()
class ACTIONROGUELIKE_API ASAICharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ASAICharacter();

	virtual void PostInitializeComponents() override;

protected:

	UPROPERTY(VisibleAnywhere, Category="Components")
	TObjectPtr<USAttributeComponent> HealthComponent;

	UPROPERTY(VisibleAnywhere, Category="Components")
	TObjectPtr<UPawnSensingComponent> PawnSensingComponent;

	UPROPERTY(VisibleAnywhere, Category="Health")
	FName HitFlashTimeParam;

	bool TrySetTargetActor(AActor* TargetActor) const;

	UFUNCTION()
	void OnPawnSeen(APawn* Pawn);

	UFUNCTION()
	void HandleHealthChanged(AActor* InstigatorActor, USAttributeComponent* OwningComponent, float Value, float Delta);
};
