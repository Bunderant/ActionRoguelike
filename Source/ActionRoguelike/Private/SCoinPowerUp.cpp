// Fill out your copyright notice in the Description page of Project Settings.


#include "SCoinPowerUp.h"

#include "SCharacter.h"
#include "SPlayerState.h"


// Sets default values
ASCoinPowerUp::ASCoinPowerUp()
{
}

bool ASCoinPowerUp::CheckCanInteract(const APawn* InstigatorPawn)
{
	return true;
}

void ASCoinPowerUp::Apply(APawn* InstigatorPawn)
{
	if (!InstigatorPawn || !InstigatorPawn->IsA(ASCharacter::StaticClass()))
	{
		return;
	}
	
	ASPlayerState* PlayerState = InstigatorPawn->GetPlayerState<ASPlayerState>();
	if (PlayerState)
	{
		PlayerState->IncrementCredits();
		Destroy();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("No player state associated with pawn. Can't apply coin powerup."));
	}
}



