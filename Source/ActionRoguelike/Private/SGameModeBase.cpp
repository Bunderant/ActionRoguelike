﻿// Fill out your copyright notice in the Description page of Project Settings.


#include "SGameModeBase.h"

#include "EngineUtils.h"
#include "SAttributeComponent.h"
#include "SCharacter.h"
#include "SMonsterData.h"
#include "SPlayerState.h"
#include "ActionRoguelike/ActionRoguelike.h"
#include "Actions/SActionComponent.h"
#include "AI/SAICharacter.h"
#include "Engine/AssetManager.h"
#include "EnvironmentQuery/EnvQueryManager.h"
#include "GameFramework/GameStateBase.h"
#include "Kismet/GameplayStatics.h"
#include "Logging/StructuredLog.h"
#include "SaveGame/SSaveGameSubsystem.h"

static TAutoConsoleVariable CVarEnableBotSpawn(TEXT("su.botSpawnEnabled"), true, TEXT("Allow bot spawning at configured time interval."), ECVF_Cheat);

ASGameModeBase::ASGameModeBase()
{
	SpawnInterval = 2.0f;
}

void ASGameModeBase::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);


	// Use the supplied SaveGame slot name, if present
	const FString SaveSlot = UGameplayStatics::ParseOption(Options, "SaveGame");
	
	USSaveGameSubsystem* SaveSystem = GetGameInstance()->GetSubsystem<USSaveGameSubsystem>();
	SaveSystem->LoadSaveGame(SaveSlot);
}

void ASGameModeBase::StartPlay()
{
	Super::StartPlay();

	GetWorldTimerManager().SetTimer(SpawnTimerHandle, this, &ASGameModeBase::OnSpawnTimerElapsed, SpawnInterval, true);

	SpawnPowerUps();
}

void ASGameModeBase::HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer)
{
	// Calling Before Super:: so we set variables before 'beginplayingstate' is called in PlayerController (which is where we instantiate UI)
	USSaveGameSubsystem* SaveSystem = GetGameInstance()->GetSubsystem<USSaveGameSubsystem>();
	SaveSystem->HandleStartingNewPlayer(NewPlayer);

	// Call SUPER later, since we want to load the player state before any other initialization here
	Super::HandleStartingNewPlayer_Implementation(NewPlayer);

	// Now we're ready to override spawn location
	// Alternatively we could override core spawn location to use store locations immediately (skipping the whole 'find player start' logic)
	bool bDidOverride = SaveSystem->OverrideSpawnTransform(NewPlayer);

	UE_LOGFMT(LogGame, Log, "Did override transform? {DidOverridde}", bDidOverride);
}

ASGameModeBase* ASGameModeBase::Get(const AActor* WorldContextObject)
{
	return WorldContextObject->GetWorld()->GetAuthGameMode<ASGameModeBase>();
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

void ASGameModeBase::SpawnPowerUps()
{
	if (NumStartupCoinPowerUps == 0 && NumStartupHealthPowerUps == 0)
	{
		return;
	}

	if (!ensureMsgf(PowerUpSpawnEnvQuery, TEXT("No power up spawn query assigned in game mode."))) return;
	
	FEnvQueryRequest Request(PowerUpSpawnEnvQuery, this);
	Request.Execute(EEnvQueryRunMode::AllMatching, this, &ASGameModeBase::OnPowerUpSpawnQueryCompleted);
}

void ASGameModeBase::OnPowerUpSpawnQueryCompleted(TSharedPtr<FEnvQueryResult> Result)
{
	if (!ensureAlwaysMsgf(!(NumStartupCoinPowerUps == 0 && NumStartupHealthPowerUps == 0), TEXT("Never run power up query if no powerups are requested.")))
	{
		return;
	}
	
	TArray<FVector> Locations;
	Result->GetAllAsLocations(Locations);

	int32 NumCoins = NumStartupCoinPowerUps;
	int32 NumHealth = NumStartupHealthPowerUps;

	if (Locations.Num() < NumStartupCoinPowerUps + NumStartupHealthPowerUps)
	{
		UE_LOG(
			LogTemp, 
			Warning, 
			TEXT("Truncating requested power-up count. EQS found %d locations, but there should be at least %d."), 
			Locations.Num(), 
			NumCoins + NumHealth);

		const float PercentCoins = NumCoins / (NumCoins + NumHealth);
		const float PercentHealth = NumHealth / (NumCoins + NumHealth);

		NumCoins = FMath::RoundToInt32(NumCoins * PercentCoins);
		NumHealth = FMath::RoundToInt32(NumHealth * PercentHealth);
	}

	// Shuffle the locations array so we get a random spread of the included power up types
	for (int32 i = 0, LastIndex = Locations.Num() - 1; i <= LastIndex; i++)
	{
		int32 RandomIndex = FMath::RandRange(0, LastIndex);
		if (RandomIndex == i) continue;

		const FVector TempLocation = Locations[i];
		Locations[i] = Locations[RandomIndex];
		Locations[RandomIndex] = TempLocation;
	}

	// Share an index across all spawn loops for each power up type
	int SharedIndex = 0;
	if (ensure(CoinPowerUpClass))
	{
		while (SharedIndex <= NumCoins && SharedIndex < Locations.Num())
		{
			GetWorld()->SpawnActor<AActor>(CoinPowerUpClass, Locations[SharedIndex] + FVector(0, 0, 100), FRotator::ZeroRotator);
			SharedIndex++;
		}
	}

	if (ensure(HealthPowerUpClass))
	{
		while (SharedIndex <= NumCoins + NumHealth && SharedIndex < Locations.Num())
		{
			GetWorld()->SpawnActor<AActor>(HealthPowerUpClass, Locations[SharedIndex] + FVector(0, 0, 100), FRotator::ZeroRotator);
			SharedIndex++;
		}
	}
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
	
	// DrawDebugSphere(GetWorld(), Locations[0], 50.0f, 20, FColor::Blue, false, 60.0f);

	if (MonsterTable) // @todo: "ensure", this should always be assigned and have at least one row
	{
		TArray<FMonsterInfoRow*> Rows;
		MonsterTable->GetAllRows("", Rows);
	
		int32 MonsterIdx = FMath::RandRange(0, Rows.Num() - 1);
		const FMonsterInfoRow* Info = Rows[MonsterIdx];

		UAssetManager& Manager = UAssetManager::Get();

		LogToScreen(this, "Loading monster...", FColor::Green);
		
		TArray<FName> Bundles; // Unused, just keeping the loading API happy
		FStreamableDelegate Delegate = FStreamableDelegate::CreateUObject(this, &ASGameModeBase::OnMonsterLoaded, Info->MonsterId, Locations[0]);
		Manager.LoadPrimaryAsset(Info->MonsterId, Bundles, Delegate);
	}
}

void ASGameModeBase::OnMonsterLoaded(FPrimaryAssetId MonsterId, FVector Location)
{
	LogToScreen(this, "Finished loading.", FColor::Green);
	const UAssetManager& Manager = UAssetManager::Get();
	
	USMonsterData* MonsterData = Manager.GetPrimaryAssetObject<USMonsterData>(MonsterId);
	if (!ensure(MonsterData))
		return;
	
	AActor* MonsterInstance = GetWorld()->SpawnActor<AActor>(MonsterData->MonsterClass, Location, FRotator::ZeroRotator);
	if (MonsterInstance && MonsterData->Actions.Num() > 0)
	{
		if (USActionComponent* ActionComp = MonsterInstance->FindComponentByClass<USActionComponent>())
		{
			for (auto ActionClass : MonsterData->Actions)
			{
				ActionComp->AddAction(MonsterInstance, ActionClass);
			}
		}
	}
}

void ASGameModeBase::RespawnPlayerDelayed(AController* PlayerController)
{
	if (!ensure(PlayerController)) return;

	PlayerController->UnPossess();
	RestartPlayer(PlayerController);
}

void ASGameModeBase::OnActorKilled(AActor* Victim, AActor* Killer)
{
	if (ASCharacter* Player = Cast<ASCharacter>(Victim))
	{
		Player->UnbindInput();
		
		FTimerHandle TimerHandle_RespawnDelay;
		FTimerDelegate RespawnDelegate;
		RespawnDelegate.BindUFunction(this, "RespawnPlayerDelayed", Player->GetController());

		const float RespawnDelay = 2.0f;
		GetWorldTimerManager().SetTimer(TimerHandle_RespawnDelay, RespawnDelegate, RespawnDelay, false);
	}

	// Separate check to see if the "killer" was a player. Needs its own block to account for PvP scenarios
	if (const ASCharacter* KillerPlayer = Cast<ASCharacter>(Killer); KillerPlayer && Victim->IsA(ASAICharacter::StaticClass()))
	{
		ASPlayerState* PlayerState = KillerPlayer->GetPlayerState<ASPlayerState>();
		PlayerState->IncrementCredits(1);
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
