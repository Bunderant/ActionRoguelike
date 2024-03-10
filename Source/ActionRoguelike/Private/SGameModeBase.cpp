// Fill out your copyright notice in the Description page of Project Settings.


#include "SGameModeBase.h"

#include "EngineUtils.h"
#include "SAttributeComponent.h"
#include "SCharacter.h"
#include "AI/SAICharacter.h"
#include "EnvironmentQuery/EnvQueryManager.h"

static TAutoConsoleVariable CVarEnableBotSpawn(TEXT("su.botSpawnEnabled"), true, TEXT("Allow bot spawning at configured time interval."), ECVF_Cheat);

ASGameModeBase::ASGameModeBase()
{
	SpawnInterval = 2.0f;
}

void ASGameModeBase::StartPlay()
{
	Super::StartPlay();

	GetWorldTimerManager().SetTimer(SpawnTimerHandle, this, &ASGameModeBase::OnSpawnTimerElapsed, SpawnInterval, true);
}

void ASGameModeBase::OnSpawnTimerElapsed()
{
	if (!CVarEnableBotSpawn->GetBool())
	{
		UE_LOG(LogTemp, Warning, TEXT("Bot spawning disabled via CVar."))
		return;
	}
	
	if (!ensureMsgf(SpawnEnvQuery, TEXT("Spawn Env Query not assigned to game mode.")))
	{
		GetWorldTimerManager().ClearTimer(SpawnTimerHandle);
		return;
	}

	int32 NumAlive = 0;
	
	// Only spawn bots if we haven't hit the threshold for the max num alive simultaneously
	for (TActorIterator<ASAICharacter> It(GetWorld()); It; ++It)
	{
		const ASAICharacter* Bot = *It;
		const USAttributeComponent* HealthAttribute = Bot->FindComponentByClass<USAttributeComponent>();
		if (ensure(HealthAttribute) && HealthAttribute->IsAlive())
		{
			NumAlive++;
		}
	}

	float MaxNumBots = 10.0f;
	if (BotMaxCountCurve)
	{
		MaxNumBots = BotMaxCountCurve->GetFloatValue(GetWorld()->TimeSeconds);
	}

	if (NumAlive >= MaxNumBots)
	{
		UE_LOG(LogTemp, Warning, TEXT("Maximum number of bots reached: %f. Retrying shortly..."), MaxNumBots);
		return;
	}
	
	FEnvQueryRequest Request(SpawnEnvQuery, this);
	Request.Execute(EEnvQueryRunMode::RandomBest5Pct, this, &ASGameModeBase::OnBotSpawnQueryCompleted);
}

void ASGameModeBase::OnBotSpawnQueryCompleted(TSharedPtr<FEnvQueryResult> Result)
{
	if (Result == nullptr || !Result->IsSuccessful())
	{
		UE_LOG(LogTemp, Warning, TEXT("Bot spawn environment query failed."));
		return;
	}

	TArray<FVector> Locations;
	Result->GetAllAsLocations(Locations);

	if (!Locations.IsValidIndex(0))
	{
		return;
	}

	DrawDebugSphere(GetWorld(), Locations[0], 50.0f, 20, FColor::Blue, false, 60.0f);
	
	GetWorld()->SpawnActor<AActor>(BotClass, Locations[0], FRotator::ZeroRotator);
}

void ASGameModeBase::RespawnPlayerDelayed(AController* PlayerController)
{
	if (!ensure(PlayerController)) return;

	PlayerController->UnPossess();
	RestartPlayer(PlayerController);
}

void ASGameModeBase::OnActorKilled(AActor* Victim, AActor* Killer)
{
	if (const ASCharacter* Player = Cast<ASCharacter>(Victim))
	{
		FTimerHandle TimerHandle_RespawnDelay;
		FTimerDelegate RespawnDelegate;
		RespawnDelegate.BindUFunction(this, "RespawnPlayerDelayed", Player->GetController());

		float RespawnDelay = 2.0f;
		GetWorldTimerManager().SetTimer(TimerHandle_RespawnDelay, RespawnDelegate, RespawnDelay, false);
	}
}

void ASGameModeBase::KillAll()
{
	// Only spawn bots if we haven't hit the threshold for the max num alive simultaneously
	for (TActorIterator<ASAICharacter> It(GetWorld()); It; ++It)
	{
		const ASAICharacter* Bot = *It;
		USAttributeComponent* HealthAttribute = Bot->FindComponentByClass<USAttributeComponent>();
		if (ensure(HealthAttribute) && HealthAttribute->IsAlive())
		{
			HealthAttribute->Kill(this); // @fixme: use player as instigator rather than game mode instance
		}
	}
}
