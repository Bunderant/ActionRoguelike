// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SInteractionComponent.generated.h"


class USWorldCommonUserWidget;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ACTIONROGUELIKE_API USInteractionComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	USInteractionComponent();
	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void FindBestInteractable();
	
	void PrimaryInteract();

protected:
	UFUNCTION(Server, Reliable)
	void ServerInteract(AActor* InFocusedActor);
	
	UPROPERTY(EditDefaultsOnly, Category="UI")
	TSubclassOf<USWorldCommonUserWidget> DefaultFocusWidget;

	UPROPERTY(EditAnywhere, Category="Trace")
	float TraceDistance;

	UPROPERTY(EditAnywhere, Category="Trace")
	float TraceRadius;

	UPROPERTY(EditAnywhere, Category="Trace")
	TEnumAsByte<ECollisionChannel> TraceCollisionChannel;

	UPROPERTY()
	TObjectPtr<USWorldCommonUserWidget> FocusWidgetInstance;
	
	UPROPERTY()
	TObjectPtr<AActor> FocusedActor;
};
