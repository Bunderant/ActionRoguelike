// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SProjectileBase.generated.h"

class USphereComponent;
class UProjectileMovementComponent;

UCLASS(Blueprintable, Abstract)
class ACTIONROGUELIKE_API ASProjectileBase : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ASProjectileBase();

protected:

	UPROPERTY(EditDefaultsOnly)
	bool bShouldBindOverlap;

	UPROPERTY(EditDefaultsOnly)
	bool bShouldBindHit;

	UPROPERTY(EditAnywhere)
	float CameraShakeRadius;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USphereComponent* SphereComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UProjectileMovementComponent* MovementComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UParticleSystemComponent* EffectComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UParticleSystem> HitParticleSystemAsset;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<UCameraShakeBase> CameraShakeAsset;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<USoundBase> Sound;

	UPROPERTY(Transient)
	TObjectPtr<UAudioComponent> AudioComponent;
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	virtual void PostInitializeComponents() override;

	virtual void Destroyed() override;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, meta=(DisplayName="Handle Projectile Overlap"))
	void HandleProjectileOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, meta=(DisplayName="Handle Projectile Hit"))
	void HandleProjectileHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	void DefaultHit(const AActor* OtherActor, const FHitResult& Hit);
};
