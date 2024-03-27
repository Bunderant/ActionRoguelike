// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SAICharacter.generated.h"

class USActionComponent;
class USWorldCommonUserWidget;
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

	UPROPERTY(EditDefaultsOnly, Category="UI")
	TSubclassOf<UUserWidget> HealthWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category="UI")
	TSubclassOf<UUserWidget> AlertWidgetClass;

	UPROPERTY()
	TObjectPtr<USWorldCommonUserWidget> HealthWidgetInstance;

	UPROPERTY()
	TObjectPtr<USWorldCommonUserWidget> AlertWidgetInstance;

	UPROPERTY(VisibleAnywhere, Category="Components")
	TObjectPtr<USActionComponent> ActionComponent;

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

	UFUNCTION(NetMulticast, Unreliable)
	void MulticastNotifyPawnSeen();
};
