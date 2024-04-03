// Fill out your copyright notice in the Description page of Project Settings.


#include "SPlayerController.h"

#include "SPlayerState.h"

void ASPlayerController::BeginPlayingState()
{
	Super::BeginPlayingState();

	BlueprintOnPlayerStateReady(GetPlayerState<ASPlayerState>());
}

void ASPlayerController::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	BlueprintOnPlayerStateReady(GetPlayerState<ASPlayerState>());
}
