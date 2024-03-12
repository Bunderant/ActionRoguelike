// Fill out your copyright notice in the Description page of Project Settings.


#include "Actions/SAction.h"

void USAction::StartAction_Implementation(AActor* Instigator)
{
	UE_LOG(LogTemp, Warning, TEXT("Action START: %s"), *GetNameSafe(this));
}

void USAction::StopAction_Implementation(AActor* Instigator)
{
	UE_LOG(LogTemp, Warning, TEXT("Action STOP: %s"), *GetNameSafe(this));
}
