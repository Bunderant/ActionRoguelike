// Fill out your copyright notice in the Description page of Project Settings.


#include "SInteractionComponent.h"

#include "SGameplayInterface.h"


// Sets default values for this component's properties
USInteractionComponent::USInteractionComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

void USInteractionComponent::PrimaryInteract()
{
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

		if (bDidHit)
		{
			DrawDebugSphere(GetWorld(), HitResult.Location, SphereRadius, 8, FColor::Green, false, 2.0f, 0, 2.0f);
			ISGameplayInterface::Execute_Interact(HitActor, Cast<APawn>(GetOwner()));
			break;
		}
	}

	DrawDebugLine(GetWorld(), EyeLocation, End, bDidHit ? FColor::Green : FColor::Red, false, 2.0f, 0, 2.0f);
}


// Called when the game starts
void USInteractionComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void USInteractionComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                           FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

