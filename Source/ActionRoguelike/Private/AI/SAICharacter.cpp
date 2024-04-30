// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/SAICharacter.h"

#include "AIController.h"
#include "BrainComponent.h"
#include "SAttributeComponent.h"
#include "SCharacter.h"
#include "Actions/SActionComponent.h"
#include "AI/SAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Blueprint/UserWidget.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Perception/PawnSensingComponent.h"
#include "UI/SWorldCommonUserWidget.h"


// Sets default values
ASAICharacter::ASAICharacter()
{
	ActionComponent = CreateDefaultSubobject<USActionComponent>("ActionComponent");
	HealthComponent = CreateDefaultSubobject<USAttributeComponent>("HealthComponent");
	PawnSensingComponent = CreateDefaultSubobject<UPawnSensingComponent>("PawnSensingComponent");

	HitFlashTimeParam = "TimeOfHit";
	
	GetMesh()->SetGenerateOverlapEvents(true);

	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}

void ASAICharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	PawnSensingComponent->OnSeePawn.AddDynamic(this, &ASAICharacter::OnPawnSeen);
	HealthComponent->OnHealthChanged.AddDynamic(this, &ASAICharacter::HandleHealthChanged);
}

bool ASAICharacter::TrySetTargetActor(AActor* TargetActor) const
{
	if (!IsValid(TargetActor) || !TargetActor->IsA(ASCharacter::StaticClass()))
		return false;

	AAIController* AIC = Cast<AAIController>(GetController());
	if (!AIC)
		return false;

	// @fixme: In multiplayer, the "seen" player pawn can quickly cycle between players, need more logic to govern this.
	if (const UObject* CurrentTargetActor = AIC->GetBlackboardComponent()->GetValueAsObject("TargetActor"); CurrentTargetActor == TargetActor)
		return false;
	
	AIC->GetBlackboardComponent()->SetValueAsObject("TargetActor", TargetActor);

	return true;
}

void ASAICharacter::OnPawnSeen(APawn* Pawn)
{
	if (!TrySetTargetActor(Pawn))
		return;

	if (!ensureMsgf(AlertWidgetClass, TEXT("Alert widget class unassigned.")))
		return;

	// No need to check whether we're on the server, since AI runs server-only anyway
	MulticastNotifyPawnSeen();
}

void ASAICharacter::HandleHealthChanged(AActor* InstigatorActor, USAttributeComponent* OwningComponent, float Value,
	float Delta)
{
	if (Delta >= 0.0f) return;

	GetMesh()->SetScalarParameterValueOnMaterials(HitFlashTimeParam, GetWorld()->TimeSeconds);

	if (HealthWidgetInstance == nullptr)
	{
		HealthWidgetInstance = CreateWidget<USWorldCommonUserWidget>(GetWorld(), HealthWidgetClass);
		if (HealthWidgetInstance)
		{
			HealthWidgetInstance->AttachedActor = this;
			HealthWidgetInstance->AddToViewport();
		}
	}
	
	if (Value <= 0.0f)
	{
		ASAIController* AIC = Cast<ASAIController>(GetController());
		if (AIC)
		{
			AIC->GetBrainComponent()->StopLogic("Killed");
		}

		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		GetCharacterMovement()->DisableMovement();
		
		GetMesh()->SetAllBodiesSimulatePhysics(true);
		GetMesh()->SetCollisionProfileName("Ragdoll");

		SetLifeSpan(10.0f);
	}
	else
	{
		TrySetTargetActor(InstigatorActor);
	}
}

void ASAICharacter::MulticastNotifyPawnSeen_Implementation()
{
	if (AlertWidgetInstance == nullptr)
	{
		AlertWidgetInstance = CreateWidget<USWorldCommonUserWidget>(GetWorld(), AlertWidgetClass);
		if (ensure(AlertWidgetInstance))
		{
			AlertWidgetInstance->AttachedActor = this;
		}
	}

	if (!AlertWidgetInstance->IsInViewport())
	{
		// Logic to remove the widget from the viewport after a delay is handled by the widget blueprint graph
		AlertWidgetInstance->AddToViewport();
	}
}
