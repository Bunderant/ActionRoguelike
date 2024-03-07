// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SGameplayInterface.h"
#include "GameFramework/Actor.h"
#include "SItemChest.generated.h"

UCLASS()
class ACTIONROGUELIKE_API ASItemChest : public AActor, public ISGameplayInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ASItemChest();

	UFUNCTION(BlueprintGetter)
	float GetOpenLidPitch() const;
	
	UPROPERTY(EditAnywhere, BlueprintGetter=GetOpenLidPitch, Category="Interact Config")
	float OpenLidPitch;

	virtual void Interact_Implementation(APawn* InstigatorPawn) override;

private:
	UPROPERTY(VisibleAnywhere, Category="Rendering", meta=(AllowPrivateAccess="true"))
	TObjectPtr<UStaticMeshComponent> BaseMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Rendering", meta=(AllowPrivateAccess="true"))
	TObjectPtr<UStaticMeshComponent> LidMesh;
};
