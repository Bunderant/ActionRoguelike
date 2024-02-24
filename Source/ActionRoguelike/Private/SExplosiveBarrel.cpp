﻿// Fill out your copyright notice in the Description page of Project Settings.


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
	RadialForceComp->bAutoActivate = false;
	RadialForceComp->SetActive(false);
	RadialForceComp->SetRelativeLocation(FVector(0, 0, 70));
	RadialForceComp->bIgnoreOwningActor = true;
	RadialForceComp->bImpulseVelChange = true;
	RadialForceComp->Radius = 1000.0f;
	RadialForceComp->ImpulseStrength = 1000.0f;
	RadialForceComp->DestructibleDamage = 1.0f;
	RadialForceComp->Falloff = RIF_Linear;
	RadialForceComp->AddCollisionChannelToAffect(ECC_WorldDynamic);
}

void ASExplosiveBarrel::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	
	Mesh->OnComponentHit.AddDynamic(this, &ASExplosiveBarrel::OnHit);
}

// Called when the game starts or when spawned
void ASExplosiveBarrel::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ASExplosiveBarrel::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ASExplosiveBarrel::Explode()
{
	RadialForceComp->FireImpulse();
}

void ASExplosiveBarrel::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
                              FVector NormalImpulse, const FHitResult& Hit)
{
	DrawDebugString(GetWorld(), Hit.ImpactPoint, *FString::Printf(TEXT("HIT @%f"), GetWorld()->TimeSeconds), nullptr, FColor::Cyan, 2.0f);
	Explode();
}