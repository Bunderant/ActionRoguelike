// Fill out your copyright notice in the Description page of Project Settings.


#include "Actions/SAction_ProjectileAttack.h"

#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"

USAction_ProjectileAttack::USAction_ProjectileAttack()
{
	Spawn_Delay = 0.2f;
	Spawn_Socket = "Muzzle_01";
}

void USAction_ProjectileAttack::StartAction_Implementation(AActor* Instigator)
{
	Super::StartAction_Implementation(Instigator);

	ACharacter* Character = Cast<ACharacter>(Instigator);
	if (!Character)
	{
		UE_LOG(LogTemp, Warning, TEXT("ProjectileAttack action instigator is not of type ACharacter."));
		return;
	}
	
	Character->PlayAnimMontage(AttackAnim);

	// Immediately play "cast" particles, if specified
	if (CastingParticles)
	{
		UGameplayStatics::SpawnEmitterAttached(
			CastingParticles, 
			Character->GetMesh(), 
			Spawn_Socket, 
			FVector::ZeroVector, 
			FRotator::ZeroRotator, 
			EAttachLocation::SnapToTarget);
	}

	// Only spawn the projectile on the server (projectile itself is replicated)
	if (Character->HasAuthority())
	{
		FTimerHandle Handle;
		FTimerDelegate Delegate;
		Delegate.BindUFunction(this, "AttackDelay_Elapsed", Character);

		GetWorld()->GetTimerManager().SetTimer(Handle, Delegate, Spawn_Delay, false);
	}
}

void USAction_ProjectileAttack::AttackDelay_Elapsed(ACharacter* InstigatorCharacter)
{
	if (!ensureAlways(ProjectileClass))
	{
		StopAction(InstigatorCharacter);
		return;
	}
	
	FHitResult HitResult;

	FVector HitTraceStart;
	FRotator ProjectileDirection;
	InstigatorCharacter->GetActorEyesViewPoint(HitTraceStart, ProjectileDirection);
	const FVector ProjectileDirectionVector = ProjectileDirection.Vector();
	const FVector HitTraceEnd = HitTraceStart + ProjectileDirectionVector * 10000;
	const FVector SpawnStart = InstigatorCharacter->GetMesh()->GetSocketLocation(Spawn_Socket);
	
	// Move the hit trace start point up a bit, so it starts from the center of the screen, but on the plane of the
	// spawn start location
	HitTraceStart = FVector::PointPlaneProject(HitTraceStart, SpawnStart, ProjectileDirectionVector);

	FCollisionQueryParams QueryParams = FCollisionQueryParams::DefaultQueryParam;
	QueryParams.AddIgnoredActor(InstigatorCharacter);
	
	const FVector SpawnEnd = GetWorld()->LineTraceSingleByProfile(HitResult, HitTraceStart, HitTraceEnd, "BlockAllObjects", QueryParams)
		? HitResult.ImpactPoint
		: HitTraceEnd;
	
	FTransform SpawnTransformMatrix = FTransform((SpawnEnd - SpawnStart).ToOrientationRotator(), SpawnStart);
	
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	SpawnParams.Instigator = InstigatorCharacter;
	
	GetWorld()->SpawnActor<AActor>(ProjectileClass, SpawnTransformMatrix, SpawnParams);

	StopAction(InstigatorCharacter);
}
