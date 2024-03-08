// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/SBTService_CheckHealthLow.h"

#include "AIController.h"
#include "SAttributeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"

void USBTService_CheckHealthLow::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	UBlackboardComponent* BlackboardComponent = OwnerComp.GetBlackboardComponent();
	if (!ensure(BlackboardComponent)) return;

	const AAIController* MyController = OwnerComp.GetAIOwner();
	if (!ensure(MyController)) return;

	const APawn* MyPawn = MyController->GetPawn();
	if (!ensure(MyPawn)) return;

	const USAttributeComponent* HealthAttribute = MyPawn->FindComponentByClass<USAttributeComponent>();
	if (!ensureAlwaysMsgf(HealthAttribute, TEXT("AI Pawn has no Health Attribute Component attached, can't check for low health."))) return;

	const bool bIsHealthLow = HealthAttribute->GetHealthAsPercent() <= LowHealthThresholdPercent;
	BlackboardComponent->SetValueAsBool(IsHealthLowKey.SelectedKeyName, bIsHealthLow);
}
