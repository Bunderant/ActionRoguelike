// Fill out your copyright notice in the Description page of Project Settings.


#include "SPlayerController.h"

#include "CommonUIExtensions.h"
#include "EnhancedInputComponent.h"
#include "SPlayerState.h"
#include "UI/SCommonActivatableWidget.h"

void ASPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	UEnhancedInputComponent* Input = Cast<UEnhancedInputComponent>(InputComponent);
	if (!ensureMsgf(Input, TEXT("Input component not found, or is not of type UEnhancedInputComponent.")))
	{
		return;
	}
	
	Input->BindAction(InputActionTogglePause.LoadSynchronous(), ETriggerEvent::Triggered, this, &ASPlayerController::TogglePauseMenu);
}

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

void ASPlayerController::TogglePauseMenu()
{
	if (PauseMenuInstance == nullptr)
	{
		PauseMenuInstance = UCommonUIExtensions::PushContentToLayer_ForPlayer(GetLocalPlayer(), PauseMenuLayer, PauseMenuClass.Get());
		Pause();
	}
	else
	{
		UCommonUIExtensions::PopContentFromLayer(PauseMenuInstance);
		PauseMenuInstance = nullptr;
		
		// NOTE: Not great api here, but the controller's Pause() function actually toggles the 'pause' state. 
		Pause();

		// Make sure the "game" input is restored
		SetInputMode(FInputModeGameOnly());
	}
}

void ASPlayerController::OnTogglePauseInputTriggered(const FInputActionInstance& Instance)
{
	TogglePauseMenu();
}
