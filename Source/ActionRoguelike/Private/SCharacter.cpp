// Fill out your copyright notice in the Description page of Project Settings.


#include "SCharacter.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "DrawDebugHelpers.h"
#include "EnhancedInputSubsystems.h"
#include "SAttributeComponent.h"
#include "SInteractionComponent.h"
#include "SPlayerState.h"
#include "Actions/SActionComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values
ASCharacter::ASCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>("Spring Arm");
	SpringArmComponent->SetupAttachment(RootComponent);
	SpringArmComponent->bUsePawnControlRotation = true;
	
	CameraComponent = CreateDefaultSubobject<UCameraComponent>("Camera");
	CameraComponent->SetupAttachment(SpringArmComponent);

	InteractionComponent = CreateDefaultSubobject<USInteractionComponent>("Interaction");

	HitFlashColor = FColor::Red;
	HitFlashDuration = 0.5f;
	HitFlashTimeParam = "TimeOfHit";
	HealthComponent = CreateDefaultSubobject<USAttributeComponent>(TEXT("Health Component"));

	ActionComponent = CreateDefaultSubobject<USActionComponent>("Action Component");

	GetCharacterMovement()->bOrientRotationToMovement = true;

	bUseControllerRotationYaw = false;
}

void ASCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	
	HealthComponent->OnAttributeChanged.AddDynamic(this, &ASCharacter::HandleHealthChanged);
}

void ASCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if (const ULocalPlayer* LocalPlayer = Cast<APlayerController>(Controller)->GetLocalPlayer())
	{
		if (UEnhancedInputLocalPlayerSubsystem* InputSystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			ensure(!InputMapping.IsNull());
			
			// TODO: MDA: Expose the mapping's priority as a global setting
			InputSystem->AddMappingContext(InputMapping.LoadSynchronous(), 0);
		}
	}

	if (ASPlayerState* CustomPlayerState = GetPlayerState<ASPlayerState>())
	{
		CustomPlayerState->ClearNonPersistentState();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Player state is not of type %s."), *ASPlayerState::StaticClass()->GetName());
	}
}

void ASCharacter::Move(const FInputActionInstance& Instance)
{
	const FVector2D Value = Instance.GetValue().Get<FVector2D>();

	FRotator ControlRotation = GetControlRotation();
	ControlRotation.Pitch = 0.0f;
	ControlRotation.Roll = 0.0f;

	const FVector ForwardVector = ControlRotation.Vector();
	const FVector RightVector = FRotationMatrix(ControlRotation).GetScaledAxis(EAxis::Y);

	AddMovementInput(ForwardVector, Value.Y);
	AddMovementInput(RightVector, Value.X);
}

void ASCharacter::MoveCamera(const FInputActionInstance& Instance)
{
	const FVector2D Value = Instance.GetValue().Get<FVector2D>();
	
	AddControllerPitchInput(Value.Y);
	AddControllerYawInput(Value.X);
}

void ASCharacter::PrimaryAttack(const FInputActionInstance& Instance)
{
	ActionComponent->StartActionByName(this, "PrimaryAttack");
}

void ASCharacter::SecondaryAttack(const FInputActionInstance& Instance)
{
	ActionComponent->StartActionByName(this, "SecondaryAttack");
}

void ASCharacter::UltimateAttack(const FInputActionInstance& Instance)
{
	ActionComponent->StartActionByName(this, "UltimateAttack");
}

void ASCharacter::HandleJumpInput(const FInputActionInstance& Instance)
{
	Jump();
}

void ASCharacter::HandleInteractInput(const FInputActionInstance& Instance)
{
	InteractionComponent->PrimaryInteract();
}

void ASCharacter::OnSecondaryMovementInputTriggered(const FInputActionInstance& Instance)
{
	if (Instance.GetValue().Get<bool>())
	{
		ActionComponent->StartActionByName(this, "Sprint");
	}
	else
	{
		ActionComponent->StopActionByName(this, "Sprint");
	}
}

void ASCharacter::OnParryInputTriggered(const FInputActionInstance& Instance)
{
	ActionComponent->StartActionByName(this, "Parry");
}

void ASCharacter::HandleHealthChanged(AActor* InstigatorActor, USAttributeComponent* OwningComponent, float Value,
                                      float Delta)
{
	if (Delta >= 0.0f)
	{
		return;
	}

	GetMesh()->SetScalarParameterValueOnMaterials(HitFlashTimeParam, GetWorld()->TimeSeconds);
	
	if (Value <= 0.0f)
	{
		if (const ULocalPlayer* LocalPlayer = Cast<APlayerController>(Controller)->GetLocalPlayer())
		{
			// Remove the input mapping context if it hasn't been done already.
			if (UEnhancedInputLocalPlayerSubsystem* InputSystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
				InputSystem->HasMappingContext(InputMapping.Get()))
			{
				InputSystem->RemoveMappingContext(InputMapping.Get());
			}
		}
	}
	
}

// Called every frame
void ASCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// -- Rotation Visualization -- //
	constexpr float DrawScale = 100.0f;
	constexpr float DrawOffset = 100.0f;
	constexpr float Thickness = 5.0f;

	FVector LineStart = GetActorLocation();
	// Offset to the right of pawn
	LineStart += GetActorRightVector() * DrawOffset;
	// Set line end in direction of the actor's forward
	const FVector ActorDirection_LineEnd = LineStart + (GetActorForwardVector() * DrawScale);
	// Draw Actor's Direction
	DrawDebugDirectionalArrow(GetWorld(), LineStart, ActorDirection_LineEnd, DrawScale, FColor::Yellow, false, 0.0f, 0, Thickness);

	const FVector ControllerDirection_LineEnd = LineStart + (GetControlRotation().Vector() * DrawScale);
	// Draw 'Controller' Rotation ('PlayerController' that 'possessed' this character)
	DrawDebugDirectionalArrow(GetWorld(), LineStart, ControllerDirection_LineEnd, DrawScale, FColor::Green, false, 0.0f, 0, Thickness);
}

// Called to bind functionality to input
void ASCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	checkCode(
		if (InputActionMoveHorizontal.IsNull() ||
			InputActionMoveCamera.IsNull() ||
			InputActionPrimaryAttack.IsNull() ||
			InputActionSecondaryAttack.IsNull() ||
			InputActionUltimateAttack.IsNull() ||
			InputActionJump.IsNull() ||
			InputActionInteract.IsNull() ||
			InputActionSecondaryMovement.IsNull() ||
			InputActionParry.IsNull())
		{
			GEngine->AddOnScreenDebugMessage(INDEX_NONE, 10.0f, FColor::Red, "All InputAction assets must be assigned in Blueprints that inherit SCharacter.");
			UE_LOG(LogTemp, Error, TEXT("All InputAction assets must be assigned in Blueprints that inherit SCharacter."));
			return;
		}
	);

	UEnhancedInputComponent* Input = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	
	Input->BindAction(InputActionMoveHorizontal.LoadSynchronous(), ETriggerEvent::Triggered, this, &ASCharacter::Move);
	Input->BindAction(InputActionMoveCamera.LoadSynchronous(), ETriggerEvent::Triggered, this, &ASCharacter::MoveCamera);
	Input->BindAction(InputActionPrimaryAttack.LoadSynchronous(), ETriggerEvent::Triggered, this, &ASCharacter::PrimaryAttack);
	Input->BindAction(InputActionSecondaryAttack.LoadSynchronous(), ETriggerEvent::Triggered, this, &ASCharacter::SecondaryAttack);
	Input->BindAction(InputActionUltimateAttack.LoadSynchronous(), ETriggerEvent::Triggered, this, &ASCharacter::UltimateAttack);
	Input->BindAction(InputActionJump.LoadSynchronous(), ETriggerEvent::Triggered, this, &ASCharacter::Jump);
	Input->BindAction(InputActionInteract.LoadSynchronous(), ETriggerEvent::Triggered, this, &ASCharacter::HandleInteractInput);
	Input->BindAction(InputActionSecondaryMovement.LoadSynchronous(), ETriggerEvent::Triggered, this, &ASCharacter::OnSecondaryMovementInputTriggered);
	Input->BindAction(InputActionParry.LoadSynchronous(), ETriggerEvent::Triggered, this, &ASCharacter::OnParryInputTriggered);
}

void ASCharacter::GetActorEyesViewPoint(FVector& OutLocation, FRotator& OutRotation) const
{
	OutLocation = CameraComponent->GetComponentLocation();
	OutRotation = CameraComponent->GetComponentRotation();
}

void ASCharacter::HealSelf(float Amount/*100*/)
{
	HealthComponent->ApplyHealthChange(this, Amount);
}

