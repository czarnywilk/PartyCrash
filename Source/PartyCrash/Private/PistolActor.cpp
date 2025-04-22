// Fill out your copyright notice in the Description page of Project Settings.


#include "PistolActor.h"
#include "Camera/CameraComponent.h"
#include "Kismet/KismetMathLibrary.h"
//#include "PartyCrash/PartyCrashCharacter.h"

void APistolActor::BeginPlay()
{
	Super::BeginPlay();
}

APistolActor::APistolActor()
{
	BoneNameToAttach = "pistol";
	MaxCapacity = 7;
	CurrentCapacity = 7;
	ActorName = "Pistolet";
}

void APistolActor::PrimaryAction(APartyCrashCharacter* PlayerCharacter)
{
	if (IsAiming && CurrentCapacity > 0) {
		this->SkeletalMesh->PlayAnimation(PistolShootMontage, false);

		FActorSpawnParameters SpawnInfo;
		SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		FTransform SpawnTransform = SkeletalMesh->GetSocketTransform("MuzzleFlash");

		FVector StartPosition = PlayerCharacter->GetFollowCamera()->GetComponentLocation();
		FVector EndPosition = PlayerCharacter->GetFollowCamera()->GetComponentRotation().Vector() * 500000.0 + StartPosition;
		FRotator Rotation = UKismetMathLibrary::FindLookAtRotation(StartPosition, EndPosition);
		SpawnTransform.SetRotation(FQuat(Rotation));
		auto Projectile = GetWorld()->SpawnActor<AMyProjectileActor>(ProjectileClass, SpawnTransform, SpawnInfo);
		
		PlayerCharacter->AddControllerPitchInput(-5.0f);
	}
}

void APistolActor::SecondaryAction(APartyCrashCharacter* PlayerCharacter)
{
	IsAiming = !IsAiming;
	PlayerCharacter->SetCharIsAiming(IsAiming);
	PlayerCharacter->bUseControllerRotationYaw = IsAiming;

	OwningChar = PlayerCharacter;

	if (IsAiming) {
		GetWorld()->GetTimerManager().SetTimer(PistolTimer, this,  &APistolActor::AdjustPlayerPosition, 0.01f, true);
	}
	else {
		GetWorld()->GetTimerManager().ClearTimer(PistolTimer);
	}
}

void APistolActor::AdjustPlayerPosition()
{
	FVector StartPosition = OwningChar->GetFollowCamera()->GetComponentLocation();
	FVector EndPosition = OwningChar->GetFollowCamera()->GetComponentRotation().Vector() * 5000.0 + StartPosition;
	FHitResult Result;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	Params.AddIgnoredActor(OwningChar);
	bool HitDetected = GetWorld()->LineTraceSingleByChannel(Result, StartPosition, EndPosition, ECollisionChannel::ECC_Visibility, Params);

	StartPosition = OwningChar->GetActorLocation();
	FVector EndPositionFromImpact = Result.ImpactPoint;

	FRotator PlayerRot = UKismetMathLibrary::FindLookAtRotation(StartPosition, EndPosition);
	//OwningChar->SetAimVerticalAxis(OwningChar->GetFollowCamera()->GetComponentRotation().Pitch);
	OwningChar->SetAimVerticalAxis(PlayerRot.Pitch);
	//OwningChar->GetController()->SetControlRotation(PlayerRot);
}
