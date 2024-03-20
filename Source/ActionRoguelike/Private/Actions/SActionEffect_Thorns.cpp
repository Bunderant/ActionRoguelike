// Fill out your copyright notice in the Description page of Project Settings.


#include "Actions/SActionEffect_Thorns.h"

#include "SAttributeComponent.h"
#include "Actions/SActionComponent.h"

USActionEffect_Thorns::USActionEffect_Thorns()
{
	PercentReflectedDamage = 0.1f; // 10% reflection damage by default
}

void USActionEffect_Thorns::StartAction_Implementation(AActor* Instigator)
{
	Super::StartAction_Implementation(Instigator);

	USAttributeComponent* HealthComp = USAttributeComponent::GetAttribute(GetOwningComponent()->GetOwner());
	
	if (!ensureMsgf(HealthComp, TEXT("Can't use Thorn effect on actor with no health attribute component.")))
		return;

	HealthComp->OnHealthChanged.AddDynamic(this, &USActionEffect_Thorns::OnHealthChanged);
}

void USActionEffect_Thorns::StopAction_Implementation(AActor* Instigator)
{
	Super::StopAction_Implementation(Instigator);

	USAttributeComponent* HealthComp = USAttributeComponent::GetAttribute(GetOwningComponent()->GetOwner());
	
	if (HealthComp)
		return;

	HealthComp->OnHealthChanged.RemoveDynamic(this, &USActionEffect_Thorns::OnHealthChanged);
}

void USActionEffect_Thorns::OnHealthChanged(AActor* InstigatorActor, USAttributeComponent* OwningComponent, float Value,
	float Delta)
{
	if (OwningComponent->GetOwner() == InstigatorActor || Delta >= 0.0f)
	{
		UE_LOG(LogTemp, Warning, TEXT("Self damgage or healing, skipping 'thorn' damage reflection."));
		return; // Don't reflect when you've damaged yourself (already reflected), or if the change was an increase
	}

	USAttributeComponent* InstigatorAttribute = USAttributeComponent::GetAttribute(InstigatorActor);
	if (!InstigatorAttribute)
	{
		UE_LOG(LogTemp, Warning, TEXT("Instigator has no health attribute."));
		return; // Nothing to do if the instigator can't be damaged
	}

	int32 ReflectedDamageAmount = FMath::RoundToInt32(PercentReflectedDamage * FMath::Abs(Delta));
	if (ReflectedDamageAmount < 1)
		ReflectedDamageAmount = 1; // Make sure we deal at least SOME reflected damage

	// FString DebugMsg = FString::Printf(TEXT("Reflecting damage on %s back to %s"), *OwningComponent->GetOwner()->GetName(), *InstigatorActor->GetName());
	// GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Purple, DebugMsg);
	
	InstigatorAttribute->ApplyHealthChange(InstigatorActor, -ReflectedDamageAmount);
}
