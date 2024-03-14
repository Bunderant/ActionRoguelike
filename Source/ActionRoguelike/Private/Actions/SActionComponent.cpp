// Fill out your copyright notice in the Description page of Project Settings.


#include "Actions/SActionComponent.h"

#include "Actions/SAction.h"


USActionComponent::USActionComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
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

	FString DebugMsg = GetNameSafe(GetOwner()) + " : " + ActiveGameplayTags.ToStringSimple();
	GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Green, DebugMsg);
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
			
			Action->StartAction(Instigator);
			return true;
		}
	}

	return false;
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

