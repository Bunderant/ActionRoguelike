// Fill out your copyright notice in the Description page of Project Settings.


#include "STargetDummy.h"

#include "SAttributeComponent.h"


// Sets default values
ASTargetDummy::ASTargetDummy()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>("Mesh Component");
	RootComponent = MeshComponent;

	HealthAttributeComponent = CreateDefaultSubobject<USAttributeComponent>("Health Attribute");
}

void ASTargetDummy::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	HealthAttributeComponent->OnHealthChanged.AddDynamic(this, &ASTargetDummy::HandleHealthChanged);
}

void ASTargetDummy::HandleHealthChanged(AActor* InstigatorActor, USAttributeComponent* OwningComponent, float Value,
	float Delta)
{
	if (Delta >= 0)
	{
		return;
	}

	MeshComponent->SetScalarParameterValueOnMaterials("TimeOfHit", GetWorld()->TimeSeconds);
}

