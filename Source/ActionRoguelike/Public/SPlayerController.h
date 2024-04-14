// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonPlayerController.h"
#include "GameplayTagContainer.h"
#include "GameFramework/PlayerController.h"
#include "SPlayerController.generated.h"

class UCommonActivatableWidget;
struct FGameplayTag;
struct FInputActionInstance;
class UInputAction;
class USCommonActivatableWidget;
class ASPlayerState;
/**
 * 
 */
UCLASS()
class ACTIONROGUELIKE_API ASPlayerController : public ACommonPlayerController
{
	GENERATED_BODY()

protected:
	virtual void SetupInputComponent() override;
	
	virtual void BeginPlayingState() override;

	virtual void OnRep_PlayerState() override;

public:
	UFUNCTION(BlueprintImplementableEvent)
	void BlueprintOnPlayerStateReady(ASPlayerState* CurrentPlayerState);

	UPROPERTY(EditDefaultsOnly, Category="UI")
	FGameplayTag PauseMenuLayer;

	UPROPERTY(EditDefaultsOnly, Category="UI")
	TSoftClassPtr<USCommonActivatableWidget> PauseMenuClass;

	UPROPERTY(EditDefaultsOnly, Category="UI")
	TSoftObjectPtr<UInputAction> InputActionTogglePause;

	UFUNCTION(BlueprintCallable, Category="UI")
	void TogglePauseMenu();

private:
	TObjectPtr<UCommonActivatableWidget> PauseMenuInstance;
	
	void OnTogglePauseInputTriggered(const FInputActionInstance& Instance);
};