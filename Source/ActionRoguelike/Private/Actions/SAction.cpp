﻿// Fill out your copyright notice in the Description page of Project Settings.


#include "Actions/SAction.h"

#include "ActionRoguelike/ActionRoguelike.h"
#include "Actions/SActionComponent.h"
#include "Net/UnrealNetwork.h"

static FAutoConsoleVariable CVarLogActionEnabled(TEXT("su.logActionsVerbose"), false, TEXT("Toggles on-screen Action logging."), ECVF_Cheat);

void USAction::StartAction_Implementation(AActor* Instigator)
{
	if (CVarLogActionEnabled->GetBool())
		LogToScreen(this, FString::Printf(TEXT("Action START: %s"), *GetNameSafe(this)), FColor::Green);

	GetOwningComponent()->ActiveGameplayTags.AppendTags(GrantedTags);

	bIsRunning = true;
}

void USAction::StopAction_Implementation(AActor* Instigator)
{
	if (CVarLogActionEnabled->GetBool())
		LogToScreen(this, FString::Printf(TEXT("Action STOP: %s"), *GetNameSafe(this)), FColor::Purple);
	
	GetOwningComponent()->ActiveGameplayTags.RemoveTags(GrantedTags);

	bIsRunning = false;
}

bool USAction::CanStart_Implementation() const
{
	return !bIsRunning && !GetOwningComponent()->ActiveGameplayTags.HasAny(BlockedTags);
}

bool USAction::IsRunning() const
{
	return bIsRunning;
}

UWorld* USAction::GetWorld() const
{
	if (const UActorComponent* ActorComponent = Cast<UActorComponent>(GetOuter()))
	{
		return ActorComponent->GetWorld();
	}

	return nullptr;
}

USActionComponent* USAction::GetOwningComponent() const
{
	return Cast<USActionComponent>(GetOuter());
}

void USAction::OnRep_IsRunning()
{
	if (bIsRunning)
		StartAction(nullptr);
	else
		StopAction(nullptr);
}

void USAction::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(USAction, bIsRunning);
}
