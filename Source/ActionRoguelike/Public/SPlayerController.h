// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonPlayerController.h"
#include "GameFramework/PlayerController.h"
#include "SPlayerController.generated.h"

class ASPlayerState;
/**
 * 
 */
UCLASS()
class ACTIONROGUELIKE_API ASPlayerController : public ACommonPlayerController
{
	GENERATED_BODY()

protected:
	virtual void BeginPlayingState() override;

	virtual void OnRep_PlayerState() override;

public:
	UFUNCTION(BlueprintImplementableEvent)
	void BlueprintOnPlayerStateReady(ASPlayerState* CurrentPlayerState);
};