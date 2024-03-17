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
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Interact Config")
	float OpenLidPitch;

	virtual void Interact_Implementation(APawn* InstigatorPawn) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	UPROPERTY(ReplicatedUsing="OnRep_LidOpen", BlueprintReadOnly, Category="State") // ReplidatedUsing == RepNotify in docs
	bool bLidOpen;

	UFUNCTION()
	void OnRep_LidOpen();

private:
	UPROPERTY(VisibleAnywhere, Category="Rendering", meta=(AllowPrivateAccess="true"))
	TObjectPtr<UStaticMeshComponent> BaseMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Rendering", meta=(AllowPrivateAccess="true"))
	TObjectPtr<UStaticMeshComponent> LidMesh;
};
