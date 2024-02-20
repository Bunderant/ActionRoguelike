// Fill out your copyright notice in the Description page of Project Settings.


#include "SExplosiveBarrel.h"

#include "PhysicsEngine/RadialForceComponent.h"


// Sets default values
ASExplosiveBarrel::ASExplosiveBarrel()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	RootComponent = Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	Mesh->SetCollisionProfileName("Destructible");

	RadialForceComp = CreateDefaultSubobject<URadialForceComponent>("Radial Force");
	
	RadialForceComp->SetupAttachment(Mesh);
	RadialForceComp->SetRelativeLocation(FVector(0, 0, 70));
	RadialForceComp->bIgnoreOwningActor = true;
	RadialForceComp->bImpulseVelChange = true;
	RadialForceComp->Radius = 1000.0f;
	RadialForceComp->ImpulseStrength = 1000.0f;
	RadialForceComp->DestructibleDamage = 1.0f;
	RadialForceComp->Falloff = RIF_Linear;
	RadialForceComp->AddCollisionChannelToAffect(ECC_PhysicsBody);
	RadialForceComp->AddCollisionChannelToAffect(ECC_Destructible);
	RadialForceComp->AddCollisionChannelToAffect(ECC_WorldDynamic);
}

// Called when the game starts or when spawned
void ASExplosiveBarrel::BeginPlay()
{
	Super::BeginPlay();

	Mesh->OnComponentHit.AddDynamic(this, &ASExplosiveBarrel::OnHit);
}

// Called every frame
void ASExplosiveBarrel::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ASExplosiveBarrel::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
                              FVector NormalImpulse, const FHitResult& Hit)
{
	UE_LOG(LogTemp, Warning, TEXT("Hit Exploding Barrel, triggering explosion..."));
	RadialForceComp->FireImpulse();
}
