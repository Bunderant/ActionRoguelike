// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SGameplayInterface.h"
#include "GameFramework/Actor.h"
#include "SPowerUpBase.generated.h"

class USInteractionComponent;

UCLASS(Abstract, Blueprintable)
class ACTIONROGUELIKE_API ASPowerUpBase : public AActor, public ISGameplayInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ASPowerUpBase();
	
	virtual void Interact_Implementation(APawn* InstigatorPawn) override;

protected:
	
	virtual bool CheckCanInteract(const APawn* InstigatorPawn);
	
	virtual void Apply(APawn* InstigatorPawn);
	
private:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess="true"))
	TObjectPtr<UStaticMeshComponent> StaticMesh;
};
