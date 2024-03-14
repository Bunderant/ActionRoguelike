// Fill out your copyright notice in the Description page of Project Settings.


#include "SInteractionComponent.h"

#include "SGameplayInterface.h"
#include "Blueprint/UserWidget.h"
#include "UI/SWorldCommonUserWidget.h"

static TAutoConsoleVariable CVarEnableInteractDebugDraw(TEXT("su.interactDebugEnabled"), false, TEXT("Draw debug shapes in the world for primary interaction input."), ECVF_Cheat);

USInteractionComponent::USInteractionComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	TraceDistance = 800.0f;
	TraceRadius = 30.0f;
	TraceCollisionChannel = ECC_WorldDynamic;
}

void USInteractionComponent::TickComponent(float DeltaTime, ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	FindBestInteractable();
}

void USInteractionComponent::FindBestInteractable()
{
	bool bShouldDebugDraw = CVarEnableInteractDebugDraw->GetBool();
	
	FCollisionObjectQueryParams ObjectQueryParams;
	ObjectQueryParams.AddObjectTypesToQuery(TraceCollisionChannel);
	
	FVector EyeLocation;
	FRotator EyeRotation;

	GetOwner()->GetActorEyesViewPoint(EyeLocation, EyeRotation);
	FVector End = EyeLocation + (EyeRotation.Vector() * TraceDistance);
	
	TArray<FHitResult> Hits;
	bool bDidHit = GetWorld()->SweepMultiByObjectType(Hits, EyeLocation, End, FQuat::Identity, ObjectQueryParams, FCollisionShape::MakeSphere(TraceRadius));

	// Clear the focused actor from the previous frame
	FocusedActor = nullptr;
	
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
				TraceRadius, 
				8, 
				bDidHit ? FColor::Green : FColor::Red, 
				false, 
				2.0f, 
				0, 
				2.0f);
		}

		if (bDidHit)
		{
			FocusedActor = HitActor;
			break;
		}
	}

	if (FocusedActor && ensure(DefaultFocusWidget))
	{
		if (!FocusWidgetInstance)
		{
			FocusWidgetInstance = Cast<USWorldCommonUserWidget>(CreateWidget(GetWorld(), DefaultFocusWidget));
		}

		FocusWidgetInstance->AttachedActor = FocusedActor;

		if (!FocusWidgetInstance->IsInViewport())
		{
			FocusWidgetInstance->AddToViewport();
		}
	}
	else if (FocusWidgetInstance && FocusWidgetInstance->IsInViewport())
	{
		FocusWidgetInstance->RemoveFromParent();
	}

	if (bShouldDebugDraw)
	{
		DrawDebugLine(GetWorld(), EyeLocation, End, bDidHit ? FColor::Green : FColor::Red, false, 2.0f, 0, 2.0f);
	}
}

void USInteractionComponent::PrimaryInteract()
{
	if (!FocusedActor) return;
	
	ISGameplayInterface::Execute_Interact(FocusedActor, Cast<APawn>(GetOwner()));
}

