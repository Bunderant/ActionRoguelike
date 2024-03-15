// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "SPlayerState.generated.h"

class USAttributeComponent;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnCreditsValueChanged, int32, NumCredits, int32, Delta);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnRageValueChanged, int32, RageAmount, int32, Delta);

/**
 * 
 */
UCLASS()
class ACTIONROGUELIKE_API ASPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	ASPlayerState();
	
	virtual void PostInitializeComponents() override;
	
	UFUNCTION(BlueprintCallable, Category="State")
	void ClearNonPersistentState();
	
	UPROPERTY(BlueprintAssignable, Category="Credits")
	FOnCreditsValueChanged OnCreditsValueChanged;
	
	UFUNCTION(BlueprintCallable, Category="Credits")
	bool IncrementCredits();
	
	UFUNCTION(BlueprintCallable, Category="Credits")
	bool DecrementCredits();
	
	UFUNCTION(BlueprintCallable, Category="Credits")
	int32 GetNumCredits() const;

	UPROPERTY(BlueprintAssignable, Category="Rage")
	FOnRageValueChanged OnRageValueChanged;
	
	UFUNCTION(BlueprintCallable, Category="Rage")
	bool IncrementRage(int32 Amount);

	UFUNCTION(BlueprintCallable, Category="Rage")
	bool DecrementRage(int32 Amount);

	UFUNCTION(BlueprintPure, Category="Rage")
	int32 GetRageAmount() const { return RageAmount; }

	UFUNCTION(BlueprintPure, Category="Rage")
	float GetRagePercent() const;
	
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Rage")
	int32 MaxRage;
	
	void SetRageAmount(const int32 Value);
	
private:
	int32 NumCredits;
	
	UPROPERTY(BlueprintGetter=GetRageAmount)
	int32 RageAmount;

	UFUNCTION()
	void HandlePawnSet(APlayerState* Player, APawn* NewPawn, APawn* OldPawn);

	UFUNCTION()
	void OnPlayerHealthChanged(AActor* InstigatorActor, USAttributeComponent* OwningComponent, float Value, float Delta);
};
