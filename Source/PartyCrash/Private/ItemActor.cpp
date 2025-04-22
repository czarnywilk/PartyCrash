// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemActor.h"
#include "PartyCrash/PartyCrashCharacter.h"

void AItemActor::Interact(APartyCrashCharacter* PlayerCharacter)
{
	FAttachmentTransformRules Rules = FAttachmentTransformRules(
		EAttachmentRule::SnapToTarget, 
		EAttachmentRule::SnapToTarget, 
		EAttachmentRule::SnapToTarget, 
		false);
	if (!IsOwned) {
		USkeletalMeshComponent* CharMesh = PlayerCharacter->GetMesh();
		if (StaticMesh->GetStaticMesh()->IsValidLowLevelFast()) {
			this->AttachToComponent(CharMesh, Rules, BoneNameToAttach);
			StaticMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}
		else if (SkeletalMesh->GetSkeletalMeshAsset()->IsValidLowLevelFast()) {
			this->AttachToComponent(CharMesh, Rules, BoneNameToAttach);
			SkeletalMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}
		else return;
		PlayerCharacter->ActorUsed = this;
		IsOwned = true;
	}
}

AItemActor::AItemActor()
{
	PrimaryActorTick.bCanEverTick = true;
	SetReplicates(true);
	bReplicates = true;
	SetReplicateMovement(true);

	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	StaticMesh->SetIsReplicated(true);
	this->SetRootComponent(StaticMesh);
	SkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMesh"));
	SkeletalMesh->SetIsReplicated(true);
	SkeletalMesh->SetupAttachment(RootComponent);

	InteractableActorWidgetComponent = CreateDefaultSubobject<UWidgetComponent>("ActorWidgetComp");
	InteractableActorWidgetComponent->SetSimulatePhysics(false);
	//InteractableActorWidgetComponent->SetAbsolute(false, false, false);
	InteractableActorWidgetComponent->SetupAttachment(GetRootComponent());
	InteractableActorWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
}

UInteractableActorWidget* AItemActor::GetInteractableActorWidget()
{
	if (Cast<UInteractableActorWidget>(InteractableActorWidgetComponent->GetWidget())) {
		GEngine->AddOnScreenDebugMessage(01, 5.0f, FColor::Red, Cast<UInteractableActorWidget>(InteractableActorWidgetComponent->GetWidget())->GetName());
		return Cast<UInteractableActorWidget>(InteractableActorWidgetComponent->GetWidget());
	}
	//GEngine->AddOnScreenDebugMessage(01, 5.0f, FColor::Red, InteractableActorWidgetComponent->GetWidget()->GetName());
	return nullptr;
}

void AItemActor::SecondaryAction_Implementation(APartyCrashCharacter* PlayerCharacter)
{
}

void AItemActor::DropItem_Implementation(APartyCrashCharacter* PlayerCharacter)
{
}

void AItemActor::PrimaryAction_Implementation(APartyCrashCharacter* PlayerCharacter)
{
}
