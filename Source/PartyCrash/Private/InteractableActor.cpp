// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractableActor.h"

// Sets default values
AInteractableActor::AInteractableActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

void AInteractableActor::Interact_Implementation(APartyCrashCharacter* PlayerCharacter)
{
}

UInteractableActorWidget* AInteractableActor::GetInteractableActorWidget()
{
	//if (InteractableActorWidgetComponent->GetWidget()->IsValidLowLevelFast())
	return Cast<UInteractableActorWidget>(InteractableActorWidgetComponent->GetWidget());
	//else return nullptr;
}

// Called when the game starts or when spawned
void AInteractableActor::BeginPlay()
{
	Super::BeginPlay();
	InteractableActorWidgetComponent->SetWidgetClass(InteractableActorWidgetClass);
	InteractableActorWidgetComponent->InitWidget();
	if (GetInteractableActorWidget()->IsValidLowLevelFast())
		GetInteractableActorWidget()->SetTextBlockText(ActorName);
}

// Called every frame
void AInteractableActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

