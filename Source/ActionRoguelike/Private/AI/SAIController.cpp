// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/SAIController.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"


// Sets default values
ASAIController::ASAIController()
{
}

// Called when the game starts or when spawned
void ASAIController::BeginPlay()
{
	Super::BeginPlay();

	RunBehaviorTree(BehaviorTree);

	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(this, 0);
	if (PlayerPawn)
	{
		GetBlackboardComponent()->SetValueAsVector("TargetLocation", PlayerPawn->GetActorLocation());
		GetBlackboardComponent()->SetValueAsObject("TargetActor", PlayerPawn);
	}
}

