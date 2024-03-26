// Fill out your copyright notice in the Description page of Project Settings.


#include "SCoinPowerUp.h"

#include "SCharacter.h"
#include "SPlayerState.h"

void ASCoinPowerUp::Apply(APawn* InstigatorPawn)
{
	if (!InstigatorPawn || !InstigatorPawn->IsA(ASCharacter::StaticClass()))
	{
		return;
	}
	
	ASPlayerState* PlayerState = InstigatorPawn->GetPlayerState<ASPlayerState>();
	if (!PlayerState)
	{
		UE_LOG(LogTemp, Warning, TEXT("No player state associated with pawn. Can't apply coin powerup."));
		return;
	}
	
	PlayerState->IncrementCredits();

	Hide();
	StartCooldownTimer();
}



