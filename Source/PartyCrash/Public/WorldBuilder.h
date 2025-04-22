// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Room.h"
#include "WorldBuilder.generated.h"

USTRUCT(BlueprintType)
struct FRoomAndFacing
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<ARoom> RoomClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int Facing = 0;

	FRoomAndFacing() {
		RoomClass = nullptr;
		Facing = 0;
	};

	FRoomAndFacing(TSubclassOf<ARoom> RoomClassParam, int FacingParam) {
		RoomClass = RoomClassParam;
		Facing = FacingParam;
	};
};

UCLASS()
class PARTYCRASH_API AWorldBuilder : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWorldBuilder();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FIntegerVector GridSize;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FIntegerVector MainRoomLocation;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	ARoom* MainRoom;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<TSubclassOf<ARoom>> MainRoomClasses;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<FIntegerVector> TakenCells;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<FRoomCell> GridCells;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<ARoom*> RoomsRefArray;

	UFUNCTION(BlueprintCallable)
	void BuildNewMap();

	UFUNCTION(BlueprintCallable)
	FVector CalculateRoomLocation(FIntegerVector GridPosition);

	UFUNCTION(BlueprintCallable)
	int CalculateIndex(FIntegerVector Location);

	UFUNCTION(BlueprintCallable)
	bool CanRoomFitIntoLocation(ARoom* Room, FIntegerVector Location);

	UFUNCTION(BlueprintCallable)
	bool CheckDoorCompability(FRoomCell FirstCell, FIntegerVector FirstLocation, FRoomCell SecondCell, FIntegerVector SecondLocation);

	UFUNCTION(BlueprintCallable)
	bool CheckWindowCompability(ARoom* Room, FIntegerVector Location);

	UFUNCTION(BlueprintCallable)
	bool CanCellFitIntoLocation(FRoomCell Cell, FIntegerVector Location);

	UFUNCTION(BlueprintCallable)
	bool CheckLocationIsInGrid(FIntegerVector Location);

	UFUNCTION(BlueprintCallable)
	TArray<FRoomAndFacing> GetRoomsThatCanFitLocation(FIntegerVector Location);

	UFUNCTION(BlueprintCallable)
	void SetRoomOnPosition(ARoom* RoomParam, FIntegerVector Location);

};
