// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "InteractionInterface.h"
#include "MainHudWidget.h"
#include "PartyCrashCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UCapsuleComponent;
class UInputMappingContext;
class UInputAction;
class AInteractableActor;
class AItemActor;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

UCLASS(config=Game)
class APartyCrashCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UCapsuleComponent* InteractionCapsule;

	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* LookAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* InteractAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* PrimaryItemAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* SecondaryItemAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* DropItemAction;
public:
	APartyCrashCharacter();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Input, Replicated)
	AInteractableActor* ActorToInteract;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Input, Replicated)
	AItemActor* ActorUsed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Input, Replicated)
	float HungerLevel;

	UPROPERTY()
	FTimerHandle HungerTimer;

protected:

	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);

	UFUNCTION(Server, Reliable)
	void Interact();

	UFUNCTION(Server, Reliable)
	void PrimaryAction();

	UFUNCTION(Server, Reliable)
	void SecondaryAction();

	UFUNCTION(Server, Reliable)
	void DropItem();
			

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	// To add mapping context
	virtual void BeginPlay();

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	UFUNCTION(BlueprintCallable, Server, Reliable)
	void CheckInteractableActor();

	UFUNCTION(Client, Reliable)
	void ChangeActorWidgetVisibility(AInteractableActor* InteractableActor, bool Show);

	UFUNCTION(Client, Reliable)
	void SwitchActorWidgetVisibility(AInteractableActor* OldActor, AInteractableActor* NewActor);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UMainHudWidget* HudWidget;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UMainHudWidget> HudWidgetClass;

	UFUNCTION(Server, Reliable)
	void SetPlayerActivityStatus(bool IsActive);

	UFUNCTION(Client, Reliable)
	void SetHudVisibility(bool IsVisible);

	UFUNCTION(Client, Reliable)
	void RefreshHudHungerLevel();

	UFUNCTION(Server, Reliable)
	void LowerHungerLevel();

	UFUNCTION(Server, Reliable)
	void ChangeHungerLevelByValue(float Diff);

	UFUNCTION(BlueprintImplementableEvent)
	void SetCharIsAiming(bool IsAiming);

	UFUNCTION(BlueprintImplementableEvent)
	void SetAimVerticalAxis(float Axis);

	UFUNCTION(BlueprintCallable)
	void Death();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};

