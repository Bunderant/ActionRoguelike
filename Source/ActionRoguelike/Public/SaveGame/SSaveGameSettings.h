// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "SSaveGameSettings.generated.h"

/**
 * 
 */
UCLASS(Config=Game, DefaultConfig, meta=(DisplayName="Save Game Settings"))
class ACTIONROGUELIKE_API USSaveGameSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category="General")
	FString SaveSlotName;

	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category="General", AdvancedDisplay)
	TSoftObjectPtr<UDataTable> DummyDataTable;

	USSaveGameSettings();
};
