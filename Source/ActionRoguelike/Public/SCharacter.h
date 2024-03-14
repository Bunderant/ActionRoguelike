// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputMappingContext.h"
#include "SCharacter.generated.h"

class USActionComponent;
class USAttributeComponent;
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

	virtual void PostInitializeComponents() override;

	
protected:
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(UIMin=0.1f, UIMax=1.0f), Category="Health")
	float HitFlashDuration;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Health")
	FColor HitFlashColor;

	UPROPERTY(VisibleAnywhere, Category="Health")
	FName HitFlashTimeParam;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Health")
	TObjectPtr<USAttributeComponent> HealthComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Actions")
	TObjectPtr<USActionComponent> ActionComponent;

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
	TSoftObjectPtr<UInputAction> InputActionUltimateAttack;

	UPROPERTY(EditDefaultsOnly, Category="Input")
	TSoftObjectPtr<UInputAction> InputActionJump;

	UPROPERTY(EditDefaultsOnly, Category="Input")
	TSoftObjectPtr<UInputAction> InputActionInteract;

	UPROPERTY(EditDefaultsOnly, Category="Input")
	TSoftObjectPtr<UInputAction> InputActionSecondaryMovement;

	UPROPERTY(EditDefaultsOnly, Category="Input")
	TSoftObjectPtr<UInputAction> InputActionParry;

	UPROPERTY(VisibleAnywhere, Category="Camera")
	TObjectPtr<USpringArmComponent> SpringArmComponent;

	UPROPERTY(VisibleAnywhere, Category="Camera")
	TObjectPtr<UCameraComponent> CameraComponent;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USInteractionComponent> InteractionComponent;
	
	virtual void PossessedBy(AController* NewController) override;

	void Move(const FInputActionInstance& Instance);
	void MoveCamera(const FInputActionInstance& Instance);
	void PrimaryAttack(const FInputActionInstance& Instance);
	void SecondaryAttack(const FInputActionInstance& Instance);
	void UltimateAttack(const FInputActionInstance& Instance);
	void HandleJumpInput(const FInputActionInstance& Instance);
	void HandleInteractInput(const FInputActionInstance& Instance);
	void OnSecondaryMovementInputTriggered(const FInputActionInstance& Instance);
	void OnParryInputTriggered(const FInputActionInstance& Instance);

	UFUNCTION()
	void HandleHealthChanged(AActor* InstigatorActor, USAttributeComponent* OwningComponent, float Value, float Delta);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void GetActorEyesViewPoint(FVector& OutLocation, FRotator& OutRotation) const override;

	UFUNCTION(Exec)
	void HealSelf(float Amount = 100);

};
