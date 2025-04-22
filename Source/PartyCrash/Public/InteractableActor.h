// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InteractableActorWidget.h"
#include "Components/WidgetComponent.h"
#include "InteractableActor.generated.h"

class APartyCrashCharacter;

UCLASS(abstract)
class PARTYCRASH_API AInteractableActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AInteractableActor();

	UFUNCTION(Server, Reliable, BlueprintCallable)
	virtual void Interact(APartyCrashCharacter* PlayerCharacter);

	UFUNCTION(BlueprintCallable)
	virtual UInteractableActorWidget* GetInteractableActorWidget();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UWidgetComponent* InteractableActorWidgetComponent;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UInteractableActorWidget> InteractableActorWidgetClass;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere)
	FString ActorName;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
