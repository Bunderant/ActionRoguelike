﻿// Fill out your copyright notice in the Description page of Project Settings.


#include "Actions/SActionComponent.h"

#include "ActionRoguelike/ActionRoguelike.h"
#include "Actions/SAction.h"


USActionComponent::USActionComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	SetIsReplicatedByDefault(true);
}

void USActionComponent::BeginPlay()
{
	Super::BeginPlay();

	for (TSubclassOf<USAction> ActionClass : DefaultActions)
	{
		AddAction(GetOwner(), ActionClass);
	}
}

void USActionComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                      FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	// FString DebugMsg = GetNameSafe(GetOwner()) + " : " + ActiveGameplayTags.ToStringSimple();
	// GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Green, DebugMsg);

	for (const USAction* Action : Actions)
	{
		const FString DebugMsg = FString::Printf(TEXT("[%s] Action: %s : IsRunning: %s : Outer: %s"),
			*GetNameSafe(GetOwner()),
			*Action->ActionName.ToString(),
			Action->IsRunning() ? TEXT("true") : TEXT("false"),
			*GetNameSafe(GetOuter()));

		LogToScreen(this, DebugMsg, Action->IsRunning() ? FColor::Blue : FColor::White, 0.0f);
	}
}

void USActionComponent::AddAction(AActor* InstigatorActor, TSubclassOf<USAction> ActionClass)
{
	if (!ensure(ActionClass)) return;
	
	TObjectPtr<USAction> Action = NewObject<USAction>(this, ActionClass);
	if (!ensure(Action)) return;

	Actions.Add(Action);

	if (Action->bAutoStart && ensure(Action->CanStart()))
	{
		Action->StartAction(InstigatorActor);
	}
}

void USActionComponent::RemoveAction(USAction* ActionToRemove)
{
	if (!ensure(ActionToRemove && !ActionToRemove->IsRunning()))
	{
		return;
	}
	
	Actions.Remove(ActionToRemove);
}

bool USActionComponent::StartActionByName(AActor* Instigator, const FName ActionName)
{
	for(TObjectPtr<USAction> Action : Actions)
	{
		if (Action && Action->ActionName == ActionName)
		{
			if (!Action->CanStart())
			{
				FString DebugMsg = GetNameSafe(GetOwner()) + " : Failed to START action " + ActionName.ToString();
				GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, DebugMsg);
				continue;
			}

			if (!GetOwner()->HasAuthority())
			{
				ServerStartAction(Instigator, ActionName);
			}
			
			Action->StartAction(Instigator);
			return true;
		}
	}

	return false;
}

void USActionComponent::ServerStartAction_Implementation(AActor* Instigator, const FName ActionName)
{
	StartActionByName(Instigator, ActionName);
}

bool USActionComponent::StopActionByName(AActor* Instigator, const FName ActionName)
{
	for(TObjectPtr<USAction> Action : Actions)
	{
		if (Action && Action->ActionName == ActionName)
		{
			if (Action->IsRunning())
			{
				Action->StopAction(Instigator);
				return true;
			}
		}
	}

	return false;
}

bool USActionComponent::CheckHasAction(const TSubclassOf<USAction> ActionClass) const
{
	for(TObjectPtr<USAction> Action : Actions)
	{
		if (Action && Action->IsA(ActionClass))
		{
			return true;
		}
	}

	return false;
}

