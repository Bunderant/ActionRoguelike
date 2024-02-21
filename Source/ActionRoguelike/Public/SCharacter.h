// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputMappingContext.h"
#include "SCharacter.generated.h"

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

	UPROPERTY(EditAnywhere)
	TSubclassOf<AActor> ProjectileClass;
	

	UPROPERTY(EditDefaultsOnly)
	TSoftObjectPtr<UInputMappingContext> InputMapping;

	UPROPERTY(EditDefaultsOnly)
	TSoftObjectPtr<UInputAction> InputActionMoveHorizontal;

	UPROPERTY(EditDefaultsOnly)
	TSoftObjectPtr<UInputAction> InputActionMoveCamera;

	UPROPERTY(EditDefaultsOnly)
	TSoftObjectPtr<UInputAction> InputActionPrimaryAttack;

	UPROPERTY(EditDefaultsOnly)
	TSoftObjectPtr<UInputAction> InputActionJump;

	UPROPERTY(EditDefaultsOnly)
	TSoftObjectPtr<UInputAction> InputActionInteract;
	

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USpringArmComponent> SpringArmComponent;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UCameraComponent> CameraComponent;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USInteractionComponent> InteractionComponent;
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void Move(const FInputActionInstance& Instance);
	void MoveCamera(const FInputActionInstance& Instance);
	void PrimaryAttack(const FInputActionInstance& Instance);
	void Jump(const FInputActionInstance& Instance);
	void HandleInteractInput(const FInputActionInstance& Instance);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
