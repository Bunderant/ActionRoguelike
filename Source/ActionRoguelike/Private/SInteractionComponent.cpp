// Fill out your copyright notice in the Description page of Project Settings.


#include "SInteractionComponent.h"

#include "SGameplayInterface.h"

static TAutoConsoleVariable CVarEnableInteractDebugDraw(TEXT("su.interactDebugEnabled"), false, TEXT("Draw debug shapes in the world for primary interaction input."), ECVF_Cheat);

void USInteractionComponent::PrimaryInteract()
{
	bool bShouldDebugDraw = CVarEnableInteractDebugDraw->GetBool();
	
	FCollisionObjectQueryParams ObjectQueryParams;
	ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldDynamic);
	
	FVector EyeLocation;
	FRotator EyeRotation;

	GetOwner()->GetActorEyesViewPoint(EyeLocation, EyeRotation);
	FVector End = EyeLocation + (EyeRotation.Vector() * 1000);

	float SphereRadius = 30.0f;
	TArray<FHitResult> Hits;
	bool bDidHit = GetWorld()->SweepMultiByObjectType(Hits, EyeLocation, End, FQuat::Identity, ObjectQueryParams, FCollisionShape::MakeSphere(SphereRadius));

	for (auto HitResult : Hits)
	{
		AActor* HitActor = HitResult.GetActor();
		
		// Make sure to null check the actor before accessing it. It's possible for it to be null sometimes even when
		// bDidHit is true.
		bDidHit &= HitActor && HitActor->Implements<USGameplayInterface>();

		if (bShouldDebugDraw)
		{
			DrawDebugSphere(
				GetWorld(), 
				HitResult.Location, 
				SphereRadius, 
				8, 
				bDidHit ? FColor::Green : FColor::Red, 
				false, 
				2.0f, 
				0, 
				2.0f);
		}

		if (bDidHit)
		{
			ISGameplayInterface::Execute_Interact(HitActor, Cast<APawn>(GetOwner()));
			break;
		}
	}

	if (bShouldDebugDraw)
	{
		DrawDebugLine(GetWorld(), EyeLocation, End, bDidHit ? FColor::Green : FColor::Red, false, 2.0f, 0, 2.0f);
	}
}

