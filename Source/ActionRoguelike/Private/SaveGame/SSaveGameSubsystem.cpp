// Fill out your copyright notice in the Description page of Project Settings.


#include "SaveGame/SSaveGameSubsystem.h"

#include "EngineUtils.h"
#include "SGameplayInterface.h"
#include "SPlayerState.h"
#include "ActionRoguelike/ActionRoguelike.h"
#include "GameFramework/GameStateBase.h"
#include "SaveGame/SSaveGameSettings.h"
#include "Kismet/GameplayStatics.h"
#include "Logging/StructuredLog.h"
#include "SaveGame/SSaveGame.h"
#include "Serialization/ObjectAndNameAsStringProxyArchive.h"

void USSaveGameSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	const USSaveGameSettings* SaveGameSettings = GetDefault<USSaveGameSettings>();

	CurrentSlotName = SaveGameSettings->SaveSlotName;

	if (SaveGameSettings->DummyDataTable)
	{
		// Force a synchronous load (further initialization of the game will likely assume this is initialized)
		UDataTable* DummyTable = SaveGameSettings->DummyDataTable.LoadSynchronous();

		// Nothing to do with the table just yet, just here as an example for reference
	}
}

void USSaveGameSubsystem::HandleStartingNewPlayer(AController* NewPlayer)
{
	ASPlayerState* PS = NewPlayer->GetPlayerState<ASPlayerState>();
	if (ensure(PS))
	{
		PS->LoadFromSavedGame(CurrentSaveGame);
	}
}

bool USSaveGameSubsystem::OverrideSpawnTransform(AController* NewPlayer)
{
	if (!IsValid(NewPlayer))
	{
		return false;
	}

	APlayerState* PlayerState = NewPlayer->GetPlayerState<APlayerState>();
	if (PlayerState == nullptr)
	{
		return false;
	}

	if (APawn* MyPawn = PlayerState->GetPawn())
	{
		FPlayerSaveData* FoundData = CurrentSaveGame->GetPlayerData(PlayerState);
		if (FoundData && FoundData->bUseSavedTransform)
		{
			MyPawn->SetActorLocationAndRotation(FoundData->Location, FoundData->Rotation);
			// Set control rotation to change camera direction, setting Pawn rotation is not enough
			NewPlayer->SetControlRotation(FoundData->Rotation);

			return true;
		}
	}

	return false;
}

void USSaveGameSubsystem::SetSlotName(FString NewSlotName)
{
	if (NewSlotName.IsEmpty())
		return;

	CurrentSlotName = NewSlotName;
}

void USSaveGameSubsystem::WriteSaveGame()
{
	if (!ensureAlwaysMsgf(IsValid(CurrentSaveGame), TEXT("Always call \"LoadGame\" before \"SaveGame\" to create the object.")))
		return;
	
	CurrentSaveGame->SavedPlayers.Empty();
	CurrentSaveGame->SavedActors.Empty();
	
	AGameStateBase* GS = GetWorld()->GetGameState();
	if (GS == nullptr)
		return;
	

	for (int32 i = 0; i < GS->PlayerArray.Num(); i++)
	{
		if (ASPlayerState* PlayerState = CastChecked<ASPlayerState>(GS->PlayerArray[i]))
		{
			PlayerState->WriteToSavedGame(CurrentSaveGame);
			break; // @todo: Update to support multiplayer
		}
	}


	for (FActorIterator It(GetWorld()); It; ++It)
	{
		AActor* Actor = *It;
		if (!IsValid(Actor) || !Actor->Implements<USGameplayInterface>())
			continue;

		FActorSaveData ActorData;
		ActorData.ActorName = Actor->GetName();
		ActorData.Transform = Actor->GetActorTransform();

		FMemoryWriter MemoryWriter(ActorData.ByteData);
		FObjectAndNameAsStringProxyArchive Ar(MemoryWriter, true);
		// Find only properties with the SaveGame specifier
		Ar.ArIsSaveGame = true;
		Actor->Serialize(Ar);

		CurrentSaveGame->SavedActors.Add(ActorData);
	}

	UGameplayStatics::SaveGameToSlot(CurrentSaveGame, CurrentSlotName, 0);
}

void USSaveGameSubsystem::LoadSaveGame(FString InSlotName)
{
	SetSlotName(InSlotName);

	if (UGameplayStatics::DoesSaveGameExist(CurrentSlotName, 0))
	{
		CurrentSaveGame = Cast<USSaveGame>(UGameplayStatics::LoadGameFromSlot(CurrentSlotName, 0));
		if (CurrentSaveGame == nullptr)
		{
			UE_LOGFMT(LogGame, Warning, "Failed to load SaveGame Data.");
			return;
		}

		UE_LOGFMT(LogGame, Log, "Loaded SaveGame Data.");

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

					FMemoryReader MemoryReader(ActorData.ByteData);
					FObjectAndNameAsStringProxyArchive Ar(MemoryReader, true);
					// Find only properties with the SaveGame specifier
					Ar.ArIsSaveGame = true;
					// Calling "serialize" with a FMemoryReader actually DE-serializes the data into the Actor's variables
					Actor->Serialize(Ar);

					ISGameplayInterface::Execute_OnActorLoaded(Actor);
					break;
				}
			}
		}

		OnSaveGameLoaded.Broadcast(CurrentSaveGame);
	}
	else
	{
		CurrentSaveGame = CastChecked<USSaveGame>(UGameplayStatics::CreateSaveGameObject(USSaveGame::StaticClass()));

		UE_LOGFMT(LogGame, Log, "Created New SaveGame Data.");
	}
}
