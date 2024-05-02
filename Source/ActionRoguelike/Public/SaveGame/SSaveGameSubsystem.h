// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "SSaveGameSubsystem.generated.h"

class USSaveGame;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSaveGameSignature, class USSaveGame*, SaveObject);

/**
 * 
 */
UCLASS(meta=(DisplayName="SaveGame Subsystem"))
class ACTIONROGUELIKE_API USSaveGameSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

protected:
	
	/* Name of slot to save/load on disk. Specified by SaveGameSettings, can be overridden by GameModeBase. */
	FString CurrentSlotName;

	UPROPERTY()
	TObjectPtr<USSaveGame> CurrentSaveGame;

public:

	void Initialize(FSubsystemCollectionBase& Collection) override;

	/* Used to restore any serialized/persistent player state. */
	void HandleStartingNewPlayer(AController* NewPlayer);

	/* Used to restore player position after loading save data. */
	UFUNCTION(BlueprintCallable, Category="SaveGame")
	bool OverrideSpawnTransform(AController* NewPlayer);

	/* Overrides the default/configured slot name for each future use (per play session) */
	UFUNCTION(BlueprintCallable, Category="SaveGame")
	void SetSlotName(FString NewSlotName);

	UFUNCTION(BlueprintCallable, Category="SaveGame")
	void WriteSaveGame();

	void LoadSaveGame(FString InSlotName = "");

	UPROPERTY(BlueprintAssignable)
	FOnSaveGameSignature OnSaveGameLoaded;

	UPROPERTY(BlueprintAssignable)
	FOnSaveGameSignature OnSaveGameWritten;
};
