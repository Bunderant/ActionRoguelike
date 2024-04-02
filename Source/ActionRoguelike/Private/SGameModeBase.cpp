// Fill out your copyright notice in the Description page of Project Settings.


#include "SGameModeBase.h"

#include "EngineUtils.h"
#include "SAttributeComponent.h"
#include "SCharacter.h"
#include "SGameplayInterface.h"
#include "SPlayerState.h"
#include "SSaveGame.h"
#include "AI/SAICharacter.h"
#include "EnvironmentQuery/EnvQueryManager.h"
#include "GameFramework/GameStateBase.h"
#include "Kismet/GameplayStatics.h"

static TAutoConsoleVariable CVarEnableBotSpawn(TEXT("su.botSpawnEnabled"), true, TEXT("Allow bot spawning at configured time interval."), ECVF_Cheat);

ASGameModeBase::ASGameModeBase()
{
	SpawnInterval = 2.0f;

	SaveSlot = "SaveSlot01";
}

void ASGameModeBase::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);

	LoadGame();
}

void ASGameModeBase::StartPlay()
{
	Super::StartPlay();

	GetWorldTimerManager().SetTimer(SpawnTimerHandle, this, &ASGameModeBase::OnSpawnTimerElapsed, SpawnInterval, true);

	SpawnPowerUps();
}

void ASGameModeBase::HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer)
{
	Super::HandleStartingNewPlayer_Implementation(NewPlayer);

	if (ASPlayerState* PlayerState = NewPlayer->GetPlayerState<ASPlayerState>())
	{
		PlayerState->LoadFromSavedGame(CurrentSaveGame);
	}
	else
	{
		UE_LOG(
			LogTemp,
			Warning,
			TEXT("[%s]: Failed to update player state from saved game data."),
			*GetNameSafe(NewPlayer));
	}
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
		PlayerState->IncrementCredits();
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

void ASGameModeBase::SaveGame()
{
	if (!ensureAlways(!SaveSlot.IsEmpty()))
		return;
	
	if (!ensureAlwaysMsgf(IsValid(CurrentSaveGame), TEXT("Always call \"LoadGame\" before \"SaveGame\" to create the object.")))
		return;

	for (int32 i = 0; i < GameState->PlayerArray.Num(); i++)
	{
		if (ASPlayerState* PlayerState = Cast<ASPlayerState>(GameState->PlayerArray[i]))
		{
			PlayerState->WriteToSavedGame(CurrentSaveGame);
			break; // @todo: Update to support multiplayer
		}
	}

	CurrentSaveGame->SavedActors.Empty();

	for (FActorIterator It(GetWorld()); It; ++It)
	{
		AActor* Actor = *It;
		if (!Actor->Implements<USGameplayInterface>())
			continue;

		FActorSaveData ActorData;
		ActorData.ActorName = Actor->GetName();
		ActorData.Transform = Actor->GetActorTransform();

		CurrentSaveGame->SavedActors.Add(ActorData);
	}

	UGameplayStatics::SaveGameToSlot(CurrentSaveGame, SaveSlot, 0);
}

void ASGameModeBase::LoadGame()
{
	if (!ensureAlways(!SaveSlot.IsEmpty()))
		return;
	
	CurrentSaveGame = UGameplayStatics::DoesSaveGameExist(SaveSlot, 0)
		? Cast<USSaveGame>(UGameplayStatics::LoadGameFromSlot(SaveSlot, 0))
		: Cast<USSaveGame>(UGameplayStatics::CreateSaveGameObject(USSaveGame::StaticClass()));

	for (FActorIterator It(GetWorld()); It; ++It)
	{
		AActor* Actor = *It;
		if (!Actor->Implements<USGameplayInterface>())
			continue;
		
		for (FActorSaveData ActorData : CurrentSaveGame->SavedActors)
		{
			if (ActorData.ActorName == Actor->GetName())
			{
				Actor->SetActorTransform(ActorData.Transform);
				break;
			}
		}
	}

	ensureAlwaysMsgf(CurrentSaveGame, TEXT("Failed to load or create game 'save' object."));
}
