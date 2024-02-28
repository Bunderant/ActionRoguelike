// Fill out your copyright notice in the Description page of Project Settings.


#include "STeleportProjectile.h"

#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Particles/ParticleSystemComponent.h"


// Sets default values
ASTeleportProjectile::ASTeleportProjectile()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	TeleportParticles = CreateDefaultSubobject<UParticleSystemComponent>("Teleport Particles");
	TeleportParticles->SetupAttachment(SphereComponent);
	TeleportParticles->bAutoActivate = false;
	TeleportParticles->SetActive(false);

	Lifetime = 0.5f;
	Delay = 0.2f;
}

// Called when the game starts or when spawned
void ASTeleportProjectile::BeginPlay()
{
	Super::BeginPlay();

	GetWorldTimerManager().SetTimer(TimerHandle_Lifetime, this, &ASTeleportProjectile::StartTeleport, Lifetime);
}

// Called every frame
void ASTeleportProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ASTeleportProjectile::HandleProjectileHit_Implementation(UPrimitiveComponent* HitComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	StartTeleport();
}

void ASTeleportProjectile::StartTeleport()
{
	SphereComponent->Deactivate();
	EffectComponent->Deactivate();
	TeleportParticles->Activate();
	MovementComponent->StopMovementImmediately();

	// Clear the "lifetime" handle explicitly in case the teleportation was triggered by a hit instead of the timer
	// expiring. 
	GetWorldTimerManager().ClearTimer(TimerHandle_Lifetime);

	GetWorldTimerManager().SetTimer(TimerHandle_Delay, this, &ASTeleportProjectile::FinishTeleport, Delay);
}

void ASTeleportProjectile::FinishTeleport()
{
	AActor* InstigatorActor = GetInstigator();
	
	if (ensure(InstigatorActor))
	{
		InstigatorActor->TeleportTo(GetActorLocation(), InstigatorActor->GetActorRotation());
	}
	
	Destroy();
}

