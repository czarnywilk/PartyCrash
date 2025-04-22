// Copyright Epic Games, Inc. All Rights Reserved.

#include "PartyCrashCharacter.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "Net/UnrealNetwork.h"
#include "InteractableActor.h"
#include "ItemActor.h"
#include "Tasks/Task.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

//////////////////////////////////////////////////////////////////////////
// APartyCrashCharacter

APartyCrashCharacter::APartyCrashCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	bReplicates = true;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	InteractionCapsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("InteractionCapsule"));

	HungerLevel = 100.0f;

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)
}

void APartyCrashCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	FTimerHandle CheckInteractableActorTimerHandle;
	GetWorldTimerManager().SetTimer(CheckInteractableActorTimerHandle, this, &APartyCrashCharacter::CheckInteractableActor, 0.2, true);

	//Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
	SetPlayerActivityStatus(true);
}


void APartyCrashCharacter::SetPlayerActivityStatus_Implementation(bool IsActive)
{
	SetHudVisibility(IsActive);
	if (IsActive) {
		GetWorld()->GetTimerManager().SetTimer(HungerTimer, this, &APartyCrashCharacter::LowerHungerLevel, 1.0f, true);
	}
	else {
		GetWorld()->GetTimerManager().ClearTimer(HungerTimer);
	}
}

void APartyCrashCharacter::SetHudVisibility_Implementation(bool IsVisible)
{
	if (IsVisible) {
		if (!HudWidget->IsValidLowLevelFast()) {
			HudWidget = CreateWidget<UMainHudWidget>(GetWorld(), HudWidgetClass);
			HudWidget->AddToViewport();
		}
		else {
			HudWidget->SetVisibility(ESlateVisibility::Visible);
		}
	}
	else if (HudWidget->IsValidLowLevelFast()) {
		HudWidget->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void APartyCrashCharacter::RefreshHudHungerLevel_Implementation()
{
	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Blue, "Lecimy z glodem");
	HudWidget->SetPercent(HungerLevel / 100.0f);
}

void APartyCrashCharacter::LowerHungerLevel_Implementation()
{
	ChangeHungerLevelByValue(-1.0f);
	GEngine->AddOnScreenDebugMessage(-1, 2.0f, GetRemoteRole() == ENetRole::ROLE_Authority ? FColor::Red : FColor::Green, FString::SanitizeFloat(HungerLevel));
	//if (this->GetController<APlayerController>()->GetLocalRole() == ROLE_Authority) HudWidget->SetPercent(HungerLevel / 100.0f);
}

void APartyCrashCharacter::ChangeHungerLevelByValue_Implementation(float Diff)
{
	this->HungerLevel += Diff;
	RefreshHudHungerLevel();
	if (GetLocalRole() == ROLE_Authority) HudWidget->SetPercent(HungerLevel / 100.0f);
	if (this->HungerLevel <= 0.0) Death();
}

void APartyCrashCharacter::Death()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Cyan, "Death");
}

void APartyCrashCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APartyCrashCharacter, ActorToInteract);
	DOREPLIFETIME(APartyCrashCharacter, HungerLevel);
}

void APartyCrashCharacter::CheckInteractableActor_Implementation()
{
	FVector StartPosition = FollowCamera->GetComponentLocation();
	FVector EndPosition = FollowCamera->GetComponentRotation().Vector() * 5000.0 + StartPosition;
	FHitResult Result;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	bool HitDetected = GetWorld()->LineTraceSingleByChannel(Result, StartPosition, EndPosition, ECollisionChannel::ECC_Visibility, Params);
	if (HitDetected) {
		AInteractableActor* ActorDetected = Cast<AInteractableActor>(Result.GetActor());
		if (!ActorDetected->IsValidLowLevelFast() && ActorToInteract->IsValidLowLevelFast()) {
			ChangeActorWidgetVisibility(ActorToInteract, false);
			ActorToInteract = nullptr;
		}
		//ActorDetected.Actor
		else if (ActorDetected != ActorToInteract) {
			SwitchActorWidgetVisibility(ActorToInteract, ActorDetected);
			ActorToInteract = ActorDetected;
			//ChangeActorWidgetVisibility(ActorToInteract, true);
		}
	}
	else if (ActorToInteract->IsValidLowLevelFast()) {
		ChangeActorWidgetVisibility(ActorToInteract, false);
		ActorToInteract = nullptr;
	}
}

void APartyCrashCharacter::ChangeActorWidgetVisibility_Implementation(AInteractableActor* InteractableActor, bool Show)
{
	if (ActorToInteract->IsValidLowLevelFast())
		Show ? ActorToInteract->GetInteractableActorWidget()->ShowTextBox() :
		ActorToInteract->GetInteractableActorWidget()->HideTextBox();
	else if (!Show) GEngine->AddOnScreenDebugMessage(-1, 5.0, FColor::Red, "Interactable Actor not found!");
}

void APartyCrashCharacter::SwitchActorWidgetVisibility_Implementation(AInteractableActor* OldActor, AInteractableActor* NewActor)
{
	if (OldActor->IsValidLowLevelFast()) OldActor->GetInteractableActorWidget()->HideTextBox();
	if (NewActor->IsValidLowLevelFast()) NewActor->GetInteractableActorWidget()->ShowTextBox();
}

//////////////////////////////////////////////////////////////////////////
// Input

void APartyCrashCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {
		//Interacting
		EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Started, this, &APartyCrashCharacter::Interact);

		EnhancedInputComponent->BindAction(PrimaryItemAction, ETriggerEvent::Started, this, &APartyCrashCharacter::PrimaryAction);

		EnhancedInputComponent->BindAction(SecondaryItemAction, ETriggerEvent::Started, this, &APartyCrashCharacter::SecondaryAction);

		EnhancedInputComponent->BindAction(DropItemAction, ETriggerEvent::Started, this, &APartyCrashCharacter::DropItem);

		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &APartyCrashCharacter::Move);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &APartyCrashCharacter::Look);
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void APartyCrashCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	
		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void APartyCrashCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void APartyCrashCharacter::PrimaryAction_Implementation()
{
	if (ActorUsed->IsValidLowLevelFast()) {
		ActorUsed->PrimaryAction(this);
	}
}

void APartyCrashCharacter::SecondaryAction_Implementation()
{
	if (ActorUsed->IsValidLowLevelFast()) {
		ActorUsed->SecondaryAction(this);
	}
}

void APartyCrashCharacter::DropItem_Implementation()
{
	if (ActorUsed->IsValidLowLevelFast()) {
		ActorUsed->DropItem(this);
	}
}

void APartyCrashCharacter::Interact_Implementation()
{
	FString Xd = "Haha";
	if (ActorToInteract->IsValidLowLevelFast()) Xd = ActorToInteract->GetName();
	UE_LOG(LogTemp, Warning, TEXT("An Actor's name is %s"), *Xd);
	if (ActorToInteract->IsValidLowLevelFast()) ActorToInteract->Interact(this);
}
