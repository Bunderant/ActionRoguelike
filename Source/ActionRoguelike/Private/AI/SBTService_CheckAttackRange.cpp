// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/SBTService_CheckAttackRange.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

void USBTService_CheckAttackRange::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	UBlackboardComponent* BlackboardComponent = OwnerComp.GetBlackboardComponent();
	if (!ensure(BlackboardComponent)) return;

	AActor* TargetActor = Cast<AActor>(BlackboardComponent->GetValueAsObject(TargetActorKey.SelectedKeyName));
	if (!TargetActor) return;

	AAIController* MyController = OwnerComp.GetAIOwner();
	if (!ensure(MyController)) return;

	APawn* MyPawn = MyController->GetPawn();
	if (!ensure(MyPawn)) return;

	float DistanceTo = TargetActor->GetDistanceTo(MyPawn);
	bool bIsWithinRange = DistanceTo <= 1000.0f;

	BlackboardComponent->SetValueAsBool(IsWithinRangeKey.SelectedKeyName, bIsWithinRange);
}
