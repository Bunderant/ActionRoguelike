// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "ModularGameMode.h"
#include "SGameModeBase.generated.h"

struct FEnvQueryResult;
class UEnvQuery;

USTRUCT(BlueprintType)
struct FMonsterInfoRow : public FTableRowBase
{
	GENERATED_BODY()

	FMonsterInfoRow()
	{
		SpawnProbabilityWeight = 1.0f;
		SpawnCost = 5.0f;
		KillReward = 20.0f;
	}
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FPrimaryAssetId MonsterId;

	/* Relative chance to pick this monster */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float SpawnProbabilityWeight;

	/* Points required by the game mode to spawn an instance */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float SpawnCost;

	/* Credits awarded to the player for killing an instance of this monster */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float KillReward;
};

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

	/* All available monsters */
	UPROPERTY(EditDefaultsOnly, Category="AI")
	TObjectPtr<UDataTable> MonsterTable;

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
	void OnMonsterLoaded(FPrimaryAssetId MonsterId, FVector Location);

	UFUNCTION()
	void RespawnPlayerDelayed(AController* PlayerController);

public:

	virtual void OnActorKilled(AActor* Victim, AActor* Killer);

	UFUNCTION(Exec)
	void KillAll();
};
