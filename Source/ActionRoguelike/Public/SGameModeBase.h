// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "SGameModeBase.generated.h"

struct FEnvQueryResult;
class UEnvQuery;
/**
 * 
 */
UCLASS()
class ACTIONROGUELIKE_API ASGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

	ASGameModeBase();

	virtual void StartPlay() override;
	
protected:

	UPROPERTY(EditAnywhere, Category="AI")
	TSubclassOf<AActor> BotClass;

	UPROPERTY(EditAnywhere, Category="AI")
	float SpawnInterval;

	UPROPERTY(EditDefaultsOnly, Category="AI")
	TObjectPtr<UEnvQuery> SpawnEnvQuery;

	FTimerHandle SpawnTimerHandle;

	void OnSpawnTimerElapsed();

	void OnBotSpawnQueryCompleted(TSharedPtr<FEnvQueryResult> Result);
};
