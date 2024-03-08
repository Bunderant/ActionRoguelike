﻿// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/SAICharacter.h"

#include "AIController.h"
#include "BrainComponent.h"
#include "SAttributeComponent.h"
#include "SCharacter.h"
#include "AI/SAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Components/CapsuleComponent.h"
#include "Perception/PawnSensingComponent.h"


// Sets default values
ASAICharacter::ASAICharacter()
{
	HealthComponent = CreateDefaultSubobject<USAttributeComponent>("HealthComponent");
	PawnSensingComponent = CreateDefaultSubobject<UPawnSensingComponent>("PawnSensingComponent");

	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}

void ASAICharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	PawnSensingComponent->OnSeePawn.AddDynamic(this, &ASAICharacter::OnPawnSeen);
	HealthComponent->OnAttributeChanged.AddDynamic(this, &ASAICharacter::HandleHealthChanged);
}

bool ASAICharacter::TrySetTargetActor(AActor* TargetActor) const
{
	if (TargetActor == nullptr || !TargetActor->IsA(ASCharacter::StaticClass()))
	{
		return false;
	}

	AAIController* AIC = Cast<AAIController>(GetController());
	if (!AIC) return false;
	
	AIC->GetBlackboardComponent()->SetValueAsObject("TargetActor", TargetActor);

	return true;
}

void ASAICharacter::OnPawnSeen(APawn* Pawn)
{
	if (!TrySetTargetActor(Pawn))
	{
		return;
	}

	DrawDebugString(GetWorld(), GetActorLocation(), "PLAYER SPOTTED", nullptr, FColor::Cyan, 0.666f, true);
}

void ASAICharacter::HandleHealthChanged(AActor* InstigatorActor, USAttributeComponent* OwningComponent, float Value,
	float Delta)
{
	if (Delta >= 0.0f) return;

	if (Value <= 0.0f)
	{
		ASAIController* AIC = Cast<ASAIController>(GetController());
		if (AIC)
		{
			AIC->GetBrainComponent()->StopLogic("Killed");
		}

		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
		GetMesh()->SetAllBodiesSimulatePhysics(true);
		GetMesh()->SetCollisionProfileName("Ragdoll");

		SetLifeSpan(10.0f);
	}
	else
	{
		TrySetTargetActor(InstigatorActor);
	}
}
