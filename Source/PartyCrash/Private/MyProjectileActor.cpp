// Fill out your copyright notice in the Description page of Project Settings.


#include "MyProjectileActor.h"
#include "PartyCrash/PartyCrashCharacter.h"

// Sets default values
AMyProjectileActor::AMyProjectileActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	ProjectileComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComponent"));
	CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComponent"));

}

// Called when the game starts or when spawned
void AMyProjectileActor::BeginPlay()
{
	Super::BeginPlay();
	CollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &AMyProjectileActor::HitFunction);
}

// Called every frame
void AMyProjectileActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AMyProjectileActor::HitFunction(UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	APartyCrashCharacter* PlayerHitted = Cast<APartyCrashCharacter>(OtherActor);
	if (PlayerHitted->IsValidLowLevelFast()) {
		PlayerHitted->Death();
	}
}

