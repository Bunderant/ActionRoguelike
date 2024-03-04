// Fill out your copyright notice in the Description page of Project Settings.


#include "SItemChest.h"


// Sets default values
ASItemChest::ASItemChest()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BaseMesh = CreateDefaultSubobject<UStaticMeshComponent>("Base Mesh");
	RootComponent = BaseMesh;

	LidMesh = CreateDefaultSubobject<UStaticMeshComponent>("Lid Mesh");
	LidMesh->SetupAttachment(BaseMesh);

	OpenLidPitch = 110;
}

void ASItemChest::Interact_Implementation(APawn* InstigatorPawn)
{
	LidMesh->SetRelativeRotation(FRotator(110, 0, 0));
}

float ASItemChest::GetOpenLidPitch() const
{
	return OpenLidPitch;
}

// Called when the game starts or when spawned
void ASItemChest::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASItemChest::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

