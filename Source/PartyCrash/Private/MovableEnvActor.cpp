// Fill out your copyright notice in the Description page of Project Settings.


#include "MovableEnvActor.h"

// Sets default values
AMovableEnvActor::AMovableEnvActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	SetReplicates(true);
	bReplicates = true;
	SetReplicateMovement(true);

	RootMesh = CreateDefaultSubobject<UStaticMeshComponent>("RootMesh");
	RootMesh->SetIsReplicated(true);
	this->SetRootComponent(RootMesh);

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("ActorMesh");
	Mesh->SetSimulatePhysics(false);
	Mesh->SetupAttachment(GetRootComponent());
	Mesh->SetIsReplicated(true);
	
	//Mesh->SetRelativeLocation(FVector(0, 0, 0));

	InteractableActorWidgetComponent = CreateDefaultSubobject<UWidgetComponent>("ActorWidgetComp");
	InteractableActorWidgetComponent->SetSimulatePhysics(false);
	//InteractableActorWidgetComponent->SetAbsolute(false, false, false);
	InteractableActorWidgetComponent->SetupAttachment(GetRootComponent());
	InteractableActorWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
	//InteractableActorWidgetComponent->SetVisibility(true);
	//InteractableActorWidgetComponent->RegisterComponent();
	
	Timeline = CreateDefaultSubobject<UTimelineComponent>("ActorTimeline");
	CanBeInterracted = true;
	IsOpened = false;
}

// Called when the game starts or when spawned
void AMovableEnvActor::BeginPlay()
{
	Super::BeginPlay();
}

void AMovableEnvActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	//Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	//DOREPLIFETIME(AInteractableActor, m_eState);
}

void AMovableEnvActor::Interact(APartyCrashCharacter* PlayerCharacter)
{
	if (CanBeInterracted) {
		CanBeInterracted = false;
		IsOpened ? Close() : Open();
	}
}

UInteractableActorWidget* AMovableEnvActor::GetInteractableActorWidget()
{
	if (Cast<UInteractableActorWidget>(InteractableActorWidgetComponent->GetWidget())){
		GEngine->AddOnScreenDebugMessage(01, 5.0f, FColor::Red, Cast<UInteractableActorWidget>(InteractableActorWidgetComponent->GetWidget())->GetName());
		return Cast<UInteractableActorWidget>(InteractableActorWidgetComponent->GetWidget());
	}
	//GEngine->AddOnScreenDebugMessage(01, 5.0f, FColor::Red, InteractableActorWidgetComponent->GetWidget()->GetName());
	return nullptr;
}

void AMovableEnvActor::EndInteraction()
{
	CanBeInterracted = true;
	IsOpened = !IsOpened;
}

void AMovableEnvActor::UpdateLocation_Implementation(FVector NewOffset, FVector NewRotatorOffset)
{

	FVector NewRelativeLocation;
	NewRelativeLocation.X = NewOffset.X * BaseOffsetMultiplier.X;
	NewRelativeLocation.Y = NewOffset.Y * BaseOffsetMultiplier.Y;
	NewRelativeLocation.Z = NewOffset.Z * BaseOffsetMultiplier.Z;

	FRotator NewRelativeRotation;
	NewRelativeRotation.Roll = NewRotatorOffset.X * BaseRotationMultiplier.X;
	NewRelativeRotation.Pitch = NewRotatorOffset.Y * BaseRotationMultiplier.Y;
	NewRelativeRotation.Yaw = NewRotatorOffset.Z * BaseRotationMultiplier.Z;

	Mesh->SetRelativeLocation(NewRelativeLocation);
	Mesh->SetRelativeRotation(NewRelativeRotation);
	FString PrintText1 = FString::SanitizeFloat(NewRelativeLocation.X);
	FString PrintText2 = FString::SanitizeFloat(NewRelativeLocation.Y);
	FString PrintText3 = FString::SanitizeFloat(NewRelativeLocation.Z);
	GEngine->AddOnScreenDebugMessage(-1, 5.0, FColor::Green, PrintText1 + " = " + PrintText2 + " * " + PrintText3);
}

// Called every frame
void AMovableEnvActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}