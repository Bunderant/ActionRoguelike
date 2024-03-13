// Fill out your copyright notice in the Description page of Project Settings.


#include "Actions/SAction.h"

#include "Actions/SActionComponent.h"

void USAction::StartAction_Implementation(AActor* Instigator)
{
	UE_LOG(LogTemp, Warning, TEXT("Action START: %s"), *GetNameSafe(this));

	GetOwningComponent()->ActiveGameplayTags.AppendTags(GrantedTags);
}

void USAction::StopAction_Implementation(AActor* Instigator)
{
	UE_LOG(LogTemp, Warning, TEXT("Action STOP: %s"), *GetNameSafe(this));
	
	GetOwningComponent()->ActiveGameplayTags.RemoveTags(GrantedTags);
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
