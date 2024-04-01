// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SSaveGame.h"
#include "GameFramework/PlayerState.h"
#include "SPlayerState.generated.h"

class USSaveGame;
class USAttributeComponent;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnCreditsValueChanged, int32, NumCredits, int32, Delta);

/**
 * 
 */
UCLASS()
class ACTIONROGUELIKE_API ASPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	ASPlayerState();

	UFUNCTION(BlueprintCallable, Category="State")
	void ClearNonPersistentState();
	
	UPROPERTY(BlueprintAssignable, Category="Credits")
	FOnCreditsValueChanged OnCreditsValueChanged;
	
	UFUNCTION(BlueprintCallable, Category="Credits")
	bool IncrementCredits();
	
	UFUNCTION(BlueprintCallable, Category="Credits")
	bool DecrementCredits();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastCreditsChanged(float Value, float Delta);
	
	UFUNCTION(BlueprintCallable, Category="Credits")
	int32 GetNumCredits() const;

	UFUNCTION(BlueprintNativeEvent, Category="Save/Load")
	void WriteToSavedGame(USSaveGame* SaveGameObject) const;

	UFUNCTION(BlueprintNativeEvent, Category="Save/Load")
	void LoadFromSavedGame(const USSaveGame* SaveGameObject);
	
protected:
	UPROPERTY(VisibleInstanceOnly, Replicated, Category="Credits")
	int32 NumCredits;
};
