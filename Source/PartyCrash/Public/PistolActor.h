// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ItemActor.h"
#include "../PartyCrashCharacter.h"
#include "MyProjectileActor.h"
#include "PistolActor.generated.h"

/**
 * 
 */
UCLASS()
class PARTYCRASH_API APistolActor : public AItemActor
{
	GENERATED_BODY()

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:

	APistolActor();

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int MaxCapacity;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int CurrentCapacity;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UAnimMontage* PistolShootMontage;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UAnimMontage* CharacterShootMontage;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FName ShootMoment;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FName MuzzleRootName;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool IsAiming;

	UPROPERTY()
	FTimerHandle PistolTimer;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	APartyCrashCharacter* OwningChar;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TSubclassOf<AMyProjectileActor> ProjectileClass;

	void PrimaryAction(APartyCrashCharacter* PlayerCharacter) override;

	void SecondaryAction(APartyCrashCharacter* PlayerCharacter) override;

	UFUNCTION()
	void AdjustPlayerPosition();
};