// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "SPlayerState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnCreditsValueChanged, int32, NumCredits, int32, Delta);

/**
 * 
 */
UCLASS()
class ACTIONROGUELIKE_API ASPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable, Category="Credits")
	FOnCreditsValueChanged OnCreditsValueChanged;
	
	UFUNCTION(BlueprintCallable, Category="Credits")
	bool IncrementCredits();
	
	UFUNCTION(BlueprintCallable, Category="Credits")
	bool DecrementCredits();

	UFUNCTION(BlueprintCallable, Category="Credits")
	int32 GetNumCredits() const;
	
private:
	int32 NumCredits;
};
