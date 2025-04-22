// Fill out your copyright notice in the Description page of Project Settings.


#include "FoodActor.h"
#include "PartyCrash/PartyCrashCharacter.h"

// Sets default values
AFoodActor::AFoodActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	ActorName = "Jedzonko";
	BoneNameToAttach = "hand_r";
	FoodPoisoned = false;
	FoodValue = 25.0f;
}

// Called when the game starts or when spawned
void AFoodActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AFoodActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AFoodActor::PrimaryAction(APartyCrashCharacter* PlayerCharacter)
{
	PlayerCharacter->ChangeHungerLevelByValue(20.0f);
	this->Destroy(true);
}

