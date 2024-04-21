// Fill out your copyright notice in the Description page of Project Settings.


#include "Actions/SActionEffect.h"

#include "Actions/SActionComponent.h"

USActionEffect::USActionEffect()
{
	Duration = 0.0f;
	Period = 0.0f;
	
	bAutoStart = true;
}

void USActionEffect::StartAction_Implementation(AActor* Instigator)
{
	Super::StartAction_Implementation(Instigator);

	if (Duration > 0)
	{
		FTimerDelegate Delegate;
		Delegate.BindUFunction(this, "StopAction", Instigator);

		GetWorld()->GetTimerManager().SetTimer(TimerHandle_Duration, Delegate, Duration, false);
	}

	if (Period > 0)
	{
		FTimerDelegate Delegate;
		Delegate.BindUFunction(this, "OnPeriodElapsed", Instigator);

		GetWorld()->GetTimerManager().SetTimer(TimerHandle_Period, Delegate, Period, true);
	}
}

void USActionEffect::StopAction_Implementation(AActor* Instigator)
{
	if (GetWorld()->GetTimerManager().GetTimerRemaining(TimerHandle_Period) <= KINDA_SMALL_NUMBER)
	{
		OnPeriodElapsed(Instigator);
	}
	
	Super::StopAction_Implementation(Instigator);

	GetWorld()->GetTimerManager().ClearTimer(TimerHandle_Duration);
	GetWorld()->GetTimerManager().ClearTimer(TimerHandle_Period);

	if (USActionComponent* Comp = GetOwningComponent())
	{
		Comp->RemoveAction(this);
	}
}

float USActionEffect::GetRemainingTime() const
{
	const float EndTime = TimeStarted + Duration;
	return EndTime - GetWorld()->TimeSeconds;
}

float USActionEffect::GetRemainingTimeNormalized() const
{
	return FMath::IsNearlyZero(Duration)
		? 0.0f
		: GetRemainingTime() / Duration;
}

void USActionEffect::OnPeriodElapsed_Implementation(AActor* Instigator)
{
}
