// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SProjectileBase.h"
#include "STeleportProjectile.generated.h"

UCLASS()
class ACTIONROGUELIKE_API ASTeleportProjectile : public ASProjectileBase
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, Category="Config", meta=(AllowPrivateAccess="true"))
	float Lifetime;

	UPROPERTY(EditAnywhere, Category="Config", meta=(AllowPrivateAccess="true"))
	float Delay;

	FTimerHandle TimerHandle_Lifetime;
	FTimerHandle TimerHandle_Delay;


public:
	// Sets default values for this actor's properties
	ASTeleportProjectile();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void HandleProjectileHit_Implementation(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) override;

private:
	void StartTeleport();
	void FinishTeleport();
};
