// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/TimelineComponent.h"
#include "Net/UnrealNetwork.h"
#include "InteractionInterface.h"
#include "Components/WidgetComponent.h"
#include "InteractableActor.h"
#include "MovableEnvActor.generated.h"

class UInteractableActorWidget;

UCLASS()
class PARTYCRASH_API AMovableEnvActor : public AInteractableActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMovableEnvActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = ())
	UStaticMeshComponent* RootMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta=())
	UStaticMeshComponent* Mesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = ())
	UTimelineComponent* Timeline;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = ())
	bool CanBeInterracted;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (), Replicated)
	bool IsOpened;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = ())
	FVector BaseOffsetMultiplier;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = ())
	FVector BaseRotationMultiplier;

	//UFUNCTION(Server, Reliable)
	void Interact(APartyCrashCharacter* PlayerCharacter) override;
	//void Interact_Implementation(APartyCrashCharacter* PlayerCharacter) override;

	UInteractableActorWidget* GetInteractableActorWidget() override;
	//UInteractableActorWidget* GetInteractableActorWidget_Implementation();

	UFUNCTION(BlueprintCallable)
	void EndInteraction();

	UFUNCTION(BlueprintImplementableEvent)
	void Open();

	UFUNCTION(BlueprintImplementableEvent)
	void Close();

	UFUNCTION(NetMulticast, Reliable, BlueprintCallable)
	void UpdateLocation(FVector NewOffset, FVector NewRotatorOffset);
};