// Fill out your copyright notice in the Description page of Project Settings.


#include "SGameplayFunctionLibrary.h"

#include "SAttributeComponent.h"

bool USGameplayFunctionLibrary::ApplyDamage(AActor* InstigatorActor, const AActor* TargetActor,
                                            const float DamageMagnitude)
{
	if (!TargetActor) return false;

	if (!ensureAlwaysMsgf(DamageMagnitude > 0.0f, TEXT("DamageMagnitude must always be > 0."))) return false;

	if (USAttributeComponent* HealthAttribute = TargetActor->FindComponentByClass<USAttributeComponent>())
	{
		return HealthAttribute->ApplyHealthChange(InstigatorActor, -DamageMagnitude);
	}
	
	return false;
}

bool USGameplayFunctionLibrary::ApplyDirectionalDamage(AActor* InstigatorActor, const AActor* TargetActor, const float DamageMagnitude,
	const FHitResult& HitResult)
{
	if (ApplyDamage(InstigatorActor, TargetActor, DamageMagnitude))
	{
		UPrimitiveComponent* PrimitiveComponent = HitResult.GetComponent();
		if (PrimitiveComponent && PrimitiveComponent->IsSimulatingPhysics(HitResult.BoneName))
		{
			PrimitiveComponent->AddImpulseAtLocation(-HitResult.ImpactNormal * 300000.f, HitResult.ImpactPoint, HitResult.BoneName);
		}
		return true;
	}

	return false;
}
