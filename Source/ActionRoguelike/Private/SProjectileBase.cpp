// Fill out your copyright notice in the Description page of Project Settings.


#include "SProjectileBase.h"

#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Particles/ParticleSystemComponent.h"


// Sets default values
ASProjectileBase::ASProjectileBase()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SphereComponent = CreateDefaultSubobject<USphereComponent>("SphereComponent");
	SphereComponent->SetCollisionProfileName("Projectile");
	SphereComponent->bAutoActivate = false;
	SphereComponent->SetActive(false);
	
	RootComponent = SphereComponent;

	EffectComponent = CreateDefaultSubobject<UParticleSystemComponent>("EffectComponent");
	EffectComponent->SetupAttachment(SphereComponent);

	MovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>("MovementComponent");
	MovementComponent->InitialSpeed = 1000.0f;
	MovementComponent->bRotationFollowsVelocity = true;
	MovementComponent->bInitialVelocityInLocalSpace = true;
	MovementComponent->ProjectileGravityScale = 0;

	bShouldBindOverlap = true;
	bShouldBindHit = true;
}

// Called when the game starts or when spawned
void ASProjectileBase::BeginPlay()
{
	Super::BeginPlay();

	// Activate the component after ignoring the instigator to avoid an immediate collision as it spawns
	SphereComponent->IgnoreActorWhenMoving(GetInstigator(), true);
	SphereComponent->Activate();
}

void ASProjectileBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (bShouldBindOverlap)
	{
		SphereComponent->OnComponentBeginOverlap.AddDynamic(this, &ASProjectileBase::HandleProjectileOverlap);
	}

	if (bShouldBindHit)
	{
		SphereComponent->OnComponentHit.AddDynamic(this, &ASProjectileBase::HandleProjectileHit);
	}
}

void ASProjectileBase::HandleProjectileOverlap_Implementation(UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
	const FHitResult& SweepResult)
{
	unimplemented();
}

void ASProjectileBase::HandleProjectileHit_Implementation(UPrimitiveComponent* HitComponent, AActor* OtherActor,
                                                          UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	unimplemented();
}

// Called every frame
void ASProjectileBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

