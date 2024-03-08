// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/SBTTask_RecoverHealth.h"

#include "AIController.h"
#include "SAttributeComponent.h"

EBTNodeResult::Type USBTTask_RecoverHealth::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	const AAIController* MyController = OwnerComp.GetAIOwner();
	if (!ensure(MyController)) return EBTNodeResult::Failed;

	APawn* MyPawn = MyController->GetPawn();
	if (!ensure(MyPawn)) return EBTNodeResult::Failed;

	USAttributeComponent* HealthAttribute = MyPawn->FindComponentByClass<USAttributeComponent>();
	if (!ensureAlwaysMsgf(HealthAttribute, TEXT("AI Pawn has no Health Attribute Component attached, can't recover health."))) return EBTNodeResult::Failed;

	DrawDebugString(GetWorld(), MyPawn->GetActorLocation(), "RECOVERED", nullptr, FColor::Cyan, 4.0f, true, 2);

	HealthAttribute->RecoverMaxHealth(MyPawn);
	return EBTNodeResult::Succeeded;
}
