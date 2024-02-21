// Fill out your copyright notice in the Description page of Project Settings.


#include "SCharacter.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInput/Public/EnhancedInputSubsystems.h"
#include "DrawDebugHelpers.h"
#include "SInteractionComponent.h"
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

	GetCharacterMovement()->bOrientRotationToMovement = true;

	bUseControllerRotationYaw = false;
}

// Called when the game starts or when spawned
void ASCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (const ULocalPlayer* LocalPlayer = Cast<APlayerController>(Controller)->GetLocalPlayer())
	{
		if (UEnhancedInputLocalPlayerSubsystem* InputSystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			ensure(!InputMapping.IsNull());
			
			// TODO: MDA: Expose the mapping's priority as a global setting
			InputSystem->AddMappingContext(InputMapping.LoadSynchronous(), 0);
		}
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
	FTransform SpawnTransformMatrix = FTransform(
		GetActorRotation(),
		GetMesh()->GetSocketLocation("Muzzle_01"));
	
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	
	GetWorld()->SpawnActor<AActor>(ProjectileClass, SpawnTransformMatrix, SpawnParams);
}

void ASCharacter::Jump(const FInputActionInstance& Instance)
{
	Super::Jump();
}

void ASCharacter::HandleInteractInput(const FInputActionInstance& Instance)
{
	InteractionComponent->PrimaryInteract();
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

	ensure(!InputActionMoveHorizontal.IsNull());
	ensure(!InputActionMoveCamera.IsNull());
	ensure(!InputActionPrimaryAttack.IsNull());
	ensure(!InputActionJump.IsNull());
	ensure(!InputActionInteract.IsNull());

	UEnhancedInputComponent* Input = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	
	Input->BindAction(InputActionMoveHorizontal.LoadSynchronous(), ETriggerEvent::Triggered, this, &ASCharacter::Move);
	Input->BindAction(InputActionMoveCamera.LoadSynchronous(), ETriggerEvent::Triggered, this, &ASCharacter::MoveCamera);
	Input->BindAction(InputActionPrimaryAttack.LoadSynchronous(), ETriggerEvent::Triggered, this, &ASCharacter::PrimaryAttack);
	Input->BindAction(InputActionJump.LoadSynchronous(), ETriggerEvent::Triggered, this, &ASCharacter::Jump);
	Input->BindAction(InputActionInteract.LoadSynchronous(), ETriggerEvent::Triggered, this, &ASCharacter::HandleInteractInput);
}

