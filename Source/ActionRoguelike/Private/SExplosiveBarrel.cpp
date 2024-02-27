// Fill out your copyright notice in the Description page of Project Settings.


#include "SExplosiveBarrel.h"

#include "SAttributeComponent.h"
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
	if (bHasAlreadyExploded)
	{
		return;
	}
	bHasAlreadyExploded = true;
	
	RadialForceComp->FireImpulse();

	TArray<FOverlapResult> HitResult;
	FCollisionShape CollisionShape = FCollisionShape::MakeSphere(RadialForceComp->Radius * 0.3333f);

	bool bDidHit = GetWorld()->OverlapMultiByProfile(HitResult, RadialForceComp->GetComponentLocation(), FQuat::Identity, "Explosion", CollisionShape);
	if (!bDidHit)
	{
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("Overlapped %d objects"), HitResult.Num());
	
	TSet<AActor*> ProcessedActors;
	bool bIsAlreadyProcessed = false;

	TArray<USAttributeComponent*, FDefaultAllocator> AttributeComponents;
	for (auto Result : HitResult)
	{
		AActor* HitActor = Result.GetActor();
		if (!HitActor)
		{
			continue;
		}

		bIsAlreadyProcessed = false;
		ProcessedActors.Add(HitActor, &bIsAlreadyProcessed);
		if (bIsAlreadyProcessed)
		{
			UE_LOG(LogTemp, Warning, TEXT("Already processed actor %s, skipping..."), *(HitActor->GetName()));
			continue;
		}

		HitActor->GetComponents(USAttributeComponent::StaticClass(), AttributeComponents);
		UE_LOG(LogTemp, Warning, TEXT("Hit actor %s with %d attributes"), *(HitActor->GetName()), AttributeComponents.Num())
		for (USAttributeComponent* HealthComponent : AttributeComponents)
		{
			HealthComponent->ApplyHealthChange(-50);
		}
	}

	Destroy();
}

void ASExplosiveBarrel::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
                              FVector NormalImpulse, const FHitResult& Hit)
{
	DrawDebugString(GetWorld(), Hit.ImpactPoint, *FString::Printf(TEXT("HIT @%f"), GetWorld()->TimeSeconds), nullptr, FColor::Cyan, 2.0f);

	Explode();
	
}
