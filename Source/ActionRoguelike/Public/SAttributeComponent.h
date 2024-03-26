// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SAttributeComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnAttributeChanged, AActor*, InstigatorActor, USAttributeComponent*, OwningComponent, float, Value, float, Delta);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ACTIONROGUELIKE_API USAttributeComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	USAttributeComponent();

	UFUNCTION(BlueprintCallable, Category="Attribute")
	static USAttributeComponent* GetAttribute(const AActor* Actor);

	UFUNCTION(BlueprintCallable, Category="Health", meta=(DisplayName="IsAlive"))
	static bool IsActorAlive(const AActor* Actor);

	UFUNCTION(BlueprintCallable, Category="Health")
	bool ApplyHealthChange(AActor* InstigatorActor, float Delta);

	UFUNCTION(BlueprintCallable, Category="Rage")
	bool ApplyRageChange(AActor* InstigatorActor, float Delta);

	UFUNCTION(BlueprintCallable, Category="Health")
	bool RecoverMaxHealth(AActor* InstigatorActor);

	UPROPERTY(BlueprintAssignable, Category="Health")
	FOnAttributeChanged OnHealthChanged;

	UPROPERTY(BlueprintAssignable, Category="Rage")
	FOnAttributeChanged OnRageChanged;

	UFUNCTION(BlueprintCallable)
	bool Kill(AActor* InstigatorActor);

	UFUNCTION(BlueprintCallable)
	bool IsAlive() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Health")
	bool IsHealthFull() const { return FMath::IsNearlyEqual(Health, MaxHealth); }

	UFUNCTION(BlueprintPure, Category="Health")
	float GetHealth() const { return Health; }

	UFUNCTION(BlueprintPure, Category="Health")
	float GetHealthAsPercent() const;

	UFUNCTION(BlueprintPure, Category="Rage")
	float GetRage() const { return Rage; }
	
	UFUNCTION(BlueprintPure, Category="Rage")
	float GetRageAsPercent() const;

protected:

	UPROPERTY(EditDefaultsOnly, BlueprintGetter="GetHealth", Replicated, Category="Health")
	float Health;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Replicated, Category="Health")
	float MaxHealth;

	UPROPERTY(EditDefaultsOnly, BlueprintGetter="GetRage", Replicated, Category="Rage")
	float Rage;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Replicated, Category="Rage")
	float MaxRage;

	UFUNCTION(NetMulticast, Reliable)
	void MulticastHealthChanged(AActor* InstigatorActor, float Value, float Delta);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastRageChanged(AActor* InstigatorActor, float Value, float Delta);
};
