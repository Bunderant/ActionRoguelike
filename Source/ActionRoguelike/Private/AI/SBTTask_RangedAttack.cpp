// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/SBTTask_RangedAttack.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Character.h"

EBTNodeResult::Type USBTTask_RangedAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* MyController = OwnerComp.GetAIOwner();
	if (!ensure(MyController)) return EBTNodeResult::Failed;

	ACharacter* MyCharacter = Cast<ACharacter>(MyController->GetPawn());
	if (MyCharacter == nullptr) return EBTNodeResult::Failed;

	UBlackboardComponent* BlackboardComponent = OwnerComp.GetBlackboardComponent();
	if (!ensure(BlackboardComponent)) return EBTNodeResult::Failed;

	AActor* TargetActor = Cast<AActor>(BlackboardComponent->GetValueAsObject("TargetActor"));
	if (TargetActor == nullptr) return EBTNodeResult::Failed;

	const FVector MuzzleLocation = MyCharacter->GetMesh()->GetSocketLocation("Muzzle_01");
	const FVector Direction = TargetActor->GetActorLocation() - MuzzleLocation;
	const FTransform SpawnTransformMatrix = FTransform(Direction.ToOrientationRotator(), MuzzleLocation);
	
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	SpawnParams.Instigator = MyCharacter;
	
	const AActor* SpawnedProjectile = GetWorld()->SpawnActor<AActor>(ProjectileClass, SpawnTransformMatrix, SpawnParams);

	return SpawnedProjectile != nullptr ? EBTNodeResult::Succeeded : EBTNodeResult::Failed;
}
