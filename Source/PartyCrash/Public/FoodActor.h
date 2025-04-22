// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ItemActor.h"
#include "FoodActor.generated.h"

UCLASS()
class PARTYCRASH_API AFoodActor : public AItemActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFoodActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void PrimaryAction(APartyCrashCharacter* PlayerCharacter) override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float FoodValue;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool FoodPoisoned;
};
