// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ModularGameMode.h"
#include "SGameModeBase.generated.h"

class USSaveGame;
struct FEnvQueryResult;
class UEnvQuery;
/**
 * 
 */
UCLASS()
class ACTIONROGUELIKE_API ASGameModeBase : public AModularGameMode
{
	GENERATED_BODY()

	ASGameModeBase();

	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;

	virtual void StartPlay() override;

	virtual void HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer) override;

public:
	UFUNCTION(BlueprintCallable, Category="Game Mode")
	static ASGameModeBase* Get(const AActor* WorldContextObject);
	
protected:

	FString SaveSlot;

	UPROPERTY()
	TObjectPtr<USSaveGame> CurrentSaveGame;

	UPROPERTY(EditAnywhere, Category="Power-ups")
	TSubclassOf<AActor> HealthPowerUpClass;

	UPROPERTY(EditAnywhere, Category="Power-ups")
	TSubclassOf<AActor> CoinPowerUpClass;

	UPROPERTY(EditAnywhere, Category="Power-ups")
	TObjectPtr<UEnvQuery> PowerUpSpawnEnvQuery;
	
	UPROPERTY(EditAnywhere, Category="Power-ups")
	int32 NumStartupHealthPowerUps;

	UPROPERTY(EditAnywhere, Category="Power-ups")
	int32 NumStartupCoinPowerUps;

	UPROPERTY(EditDefaultsOnly, Category="AI")
	TSubclassOf<AActor> BotClass;

	UPROPERTY(EditDefaultsOnly, Category="AI")
	TObjectPtr<UCurveFloat> BotMaxCountCurve;

	UPROPERTY(EditDefaultsOnly, Category="AI")
	float SpawnInterval;

	UPROPERTY(EditDefaultsOnly, Category="AI")
	TObjectPtr<UEnvQuery> SpawnEnvQuery;

	FTimerHandle SpawnTimerHandle;

	void OnSpawnTimerElapsed();

	void SpawnPowerUps();

	void OnPowerUpSpawnQueryCompleted(TSharedPtr<FEnvQueryResult> Result);

	void OnBotSpawnQueryCompleted(TSharedPtr<FEnvQueryResult> Result);

	UFUNCTION()
	void RespawnPlayerDelayed(AController* PlayerController);

public:

	virtual void OnActorKilled(AActor* Victim, AActor* Killer);

	UFUNCTION(Exec)
	void KillAll();

	UFUNCTION(BlueprintCallable, Category="Save/Load")
	void SaveGame();

	UFUNCTION()
	void LoadGame();
};
