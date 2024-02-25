// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputMappingContext.h"
#include "SCharacter.generated.h"

class ASProjectileBase;
class USpringArmComponent;
class UCameraComponent;
class USInteractionComponent;

UCLASS()
class ACTIONROGUELIKE_API ASCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ASCharacter();

protected:
	
	UPROPERTY(EditAnywhere, Category="Attack")
	TSubclassOf<AActor> PrimaryProjectileClass;

	UPROPERTY(EditAnywhere, Category="Attack")
	TSubclassOf<AActor> SecondaryProjectileClass;

	UPROPERTY(EditAnywhere, Category="Attack")
	TObjectPtr<UAnimMontage> AttackAnim;

	UPROPERTY(EditAnywhere, Category="Attack")
	FTimerHandle TimerHandle_PrimaryAttack;

	UPROPERTY(EditAnywhere, Category="Attack")
	FTimerHandle TimerHandle_SecondaryAttack;

	UPROPERTY(EditAnywhere, Category="Attack")
	float SpawnProjectile_Delay;

	UPROPERTY(EditAnywhere, Category="Attack")
	FName SpawnProjectile_Socket;
	

	UPROPERTY(EditDefaultsOnly, Category="Input")
	TSoftObjectPtr<UInputMappingContext> InputMapping;

	UPROPERTY(EditDefaultsOnly, Category="Input")
	TSoftObjectPtr<UInputAction> InputActionMoveHorizontal;

	UPROPERTY(EditDefaultsOnly, Category="Input")
	TSoftObjectPtr<UInputAction> InputActionMoveCamera;

	UPROPERTY(EditDefaultsOnly, Category="Input")
	TSoftObjectPtr<UInputAction> InputActionPrimaryAttack;

	UPROPERTY(EditDefaultsOnly, Category="Input")
	TSoftObjectPtr<UInputAction> InputActionSecondaryAttack;

	UPROPERTY(EditDefaultsOnly, Category="Input")
	TSoftObjectPtr<UInputAction> InputActionJump;

	UPROPERTY(EditDefaultsOnly, Category="Input")
	TSoftObjectPtr<UInputAction> InputActionInteract;
	

	UPROPERTY(VisibleAnywhere, Category="Camera")
	TObjectPtr<USpringArmComponent> SpringArmComponent;

	UPROPERTY(VisibleAnywhere, Category="Camera")
	TObjectPtr<UCameraComponent> CameraComponent;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USInteractionComponent> InteractionComponent;
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void Move(const FInputActionInstance& Instance);
	void MoveCamera(const FInputActionInstance& Instance);
	void PrimaryAttack(const FInputActionInstance& Instance);
	void SecondaryAttack(const FInputActionInstance& Instance);
	void SpawnPrimaryProjectile();
	void SpawnSecondaryProjectile();
	void SpawnProjectile(TSubclassOf<AActor> ProjectileClass);
	void HandleJumpInput(const FInputActionInstance& Instance);
	void HandleInteractInput(const FInputActionInstance& Instance);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
