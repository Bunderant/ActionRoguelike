// Fill out your copyright notice in the Description page of Project Settings.


#include "SProjectileBase.h"

#include "SAttributeComponent.h"
#include "Components/AudioComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
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
	DamageAmount = 20.0f;
	CameraShakeRadius = 500.0f;
}

// Called when the game starts or when spawned
void ASProjectileBase::BeginPlay()
{
	Super::BeginPlay();

	// Activate the component after ignoring the instigator to avoid an immediate collision as it spawns
	SphereComponent->IgnoreActorWhenMoving(GetInstigator(), true);
	SphereComponent->Activate();

	AudioComponent = UGameplayStatics::SpawnSoundAttached(Sound, SphereComponent);
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

void ASProjectileBase::Destroyed()
{
	Super::Destroyed();

	// "Destroyed" is called from the editor as well as Gameplay, so make sure the component has been created
	if (AudioComponent)
	{
		AudioComponent->SetTriggerParameter("Explode");
	}
}

void ASProjectileBase::HandleProjectileOverlap_Implementation(UPrimitiveComponent* OverlappedComponent,
                                                              AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                                                              const FHitResult& SweepResult)
{
	DefaultHit(OtherActor, SweepResult);
}

void ASProjectileBase::HandleProjectileHit_Implementation(UPrimitiveComponent* HitComponent, AActor* OtherActor,
                                                          UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	DefaultHit(OtherActor, Hit);
}

void ASProjectileBase::DefaultHit(const AActor* OtherActor, const FHitResult& Hit)
{
	if (!OtherActor || OtherActor == GetInstigator())
	{
		return;
	}
	
	USAttributeComponent* HealthComponent = OtherActor->FindComponentByClass<USAttributeComponent>();
	if (HealthComponent)
	{
		HealthComponent->ApplyHealthChange(GetInstigator(), -DamageAmount);
	}

	const FVector Location = Hit.bBlockingHit
		? Hit.ImpactPoint
		: GetActorLocation();
	
	const FRotator Rotation = UKismetMathLibrary::MakeRotFromX(Hit.ImpactNormal);
	
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), HitParticleSystemAsset, Location, Rotation);

	if (CameraShakeAsset)
	{
		UGameplayStatics::PlayWorldCameraShake(GetWorld(), CameraShakeAsset, Location, 0.0f, CameraShakeRadius);
	}

	Destroy();
}

