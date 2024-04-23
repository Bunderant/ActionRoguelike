// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/SAnimInstance.h"

#include "Actions/SActionComponent.h"

void USAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	if (const AActor* OwningActor = GetOwningActor())
	{
		ActionComp = OwningActor->FindComponentByClass<USActionComponent>();
	}
}

void USAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (ActionComp)
	{
		const FGameplayTag StunnedTag = FGameplayTag::RequestGameplayTag("Status.Stunned");
		bIsStunned = ActionComp->ActiveGameplayTags.HasTag(StunnedTag);
	}
}
