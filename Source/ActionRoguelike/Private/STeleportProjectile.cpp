// Fill out your copyright notice in the Description page of Project Settings.


#include "STeleportProjectile.h"

#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"


// Sets default values
ASTeleportProjectile::ASTeleportProjectile()
{
	Lifetime = 0.5f;
	Delay = 0.2f;
}

// Called when the game starts or when spawned
void ASTeleportProjectile::BeginPlay()
{
	Super::BeginPlay();

	GetWorldTimerManager().SetTimer(TimerHandle_Lifetime, this, &ASTeleportProjectile::StartTeleport, Lifetime);
}

void ASTeleportProjectile::HandleProjectileHit_Implementation(UPrimitiveComponent* HitComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	StartTeleport();
}

void ASTeleportProjectile::StartTeleport()
{
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), HitParticleSystemAsset, GetActorLocation(), GetActorRotation());
	
	SphereComponent->Deactivate();
	EffectComponent->Deactivate();
	MovementComponent->StopMovementImmediately();

	// Clear the "lifetime" handle explicitly in case the teleportation was triggered by a hit instead of the timer
	// expiring. 
	GetWorldTimerManager().ClearTimer(TimerHandle_Lifetime);

	GetWorldTimerManager().SetTimer(TimerHandle_Delay, this, &ASTeleportProjectile::FinishTeleport, Delay);
}

void ASTeleportProjectile::FinishTeleport()
{
	AActor* InstigatorActor = GetInstigator();
	
	if (ensureAlways(InstigatorActor))
	{
		InstigatorActor->TeleportTo(GetActorLocation(), InstigatorActor->GetActorRotation());
	}
	
	Destroy();
}

