// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InteractableActor.h"
#include "ItemActor.generated.h"

class APartyCrashCharacter;

/**
 * 
 */
UCLASS(abstract)
class PARTYCRASH_API AItemActor : public AInteractableActor
{
	GENERATED_BODY()
public:
	// Sets default values for this actor's properties
	void Interact(APartyCrashCharacter* PlayerCharacter) override;

	AItemActor();

	UFUNCTION(Server, Reliable)
	virtual void PrimaryAction(APartyCrashCharacter* PlayerCharacter);

	UFUNCTION(Server, Reliable)
	virtual void SecondaryAction(APartyCrashCharacter* PlayerCharacter);

	UFUNCTION(Server, Reliable)
	void DropItem(APartyCrashCharacter* PlayerCharacter);

	UInteractableActorWidget* GetInteractableActorWidget() override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FName BoneNameToAttach;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UStaticMeshComponent* StaticMesh;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	USkeletalMeshComponent* SkeletalMesh;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool IsOwned;
};
