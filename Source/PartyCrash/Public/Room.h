// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/LevelStreamingDynamic.h"
#include "Room.generated.h"

UENUM(BlueprintType)
enum class WorldDir : uint8 {
	NORTH = 0		UMETA(DisplayName = "NORTH"),
	EAST = 1		UMETA(DisplayName = "EAST"),
	SOUTH = 2		UMETA(DisplayName = "SOUTH"),
	WEST = 3		UMETA(DisplayName = "WEST")
};

USTRUCT(BlueprintType)
struct FRoomCell
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString CellName = "Cell";

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int DoorNorth = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int DoorSouth = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int DoorWest = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int DoorEast = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FName> NorthComps;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FName> EastComps;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FName> SouthComps;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FName> WestComps;

	UPROPERTY(BlueprintReadOnly)
	ARoom* RoomRef;

	FRoomCell() {
		DoorNorth = 0;
		DoorSouth = 0;
		DoorWest = 0;
		DoorEast = 0;
		RoomRef = nullptr;
	};

	FRoomCell(int NorthParam, int SouthParam, int WestParam, int EastParam) {
		DoorNorth = NorthParam;
		DoorSouth = SouthParam;
		DoorWest = WestParam;
		DoorEast = EastParam;
		RoomRef = nullptr;
	};

	void RotateCellRight() {
		FRoomCell NewCell;
		int Temp = DoorNorth;
		DoorNorth = DoorWest;
		DoorWest = DoorSouth;
		DoorSouth = DoorEast;
		DoorEast = Temp;

		TArray<FName> TempArray = NorthComps;
		NorthComps = WestComps;
		WestComps = SouthComps;
		SouthComps = EastComps;
		EastComps = TempArray;
	};

	friend bool operator==(FRoomCell const& FirstArg, FRoomCell const& Other)
	{
		return (FirstArg.NorthComps == Other.NorthComps && FirstArg.WestComps == Other.WestComps && FirstArg.SouthComps == Other.SouthComps && 
			FirstArg.EastComps == Other.EastComps && FirstArg.DoorNorth == Other.DoorNorth && FirstArg.DoorWest == Other.DoorWest
			&& FirstArg.DoorSouth == Other.DoorSouth && FirstArg.DoorEast == Other.DoorEast);
	}
};

USTRUCT(BlueprintType)
struct FIntegerVector
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int X = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int Y = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int Z = 1;

	FIntegerVector() {
		X = 1;
		Y = 1;
		Z = 1;
	};

	FIntegerVector(int XParam, int YParam, int ZParam) {
		X = XParam;
		Y = YParam;
		Z = ZParam;
	};

	FString ConvertToName() {
		return FString::FromInt(X) + "," + FString::FromInt(Y) + "," + FString::FromInt(Z);
	};

	FVector ToFloatVector() {
		return FVector(X, Y, Z);
	}

	FIntegerVector operator+(FIntegerVector const& Other)
	{
		return FIntegerVector(X + Other.X, Y + Other.Y, Z + Other.Z);
	}

	FIntegerVector operator-(FIntegerVector const& Other)
	{
		return FIntegerVector(X - Other.X, Y - Other.Y, Z - Other.Z);
	}

	FIntegerVector operator*(FIntegerVector const& Other)
	{
		return FIntegerVector(X * Other.X, Y * Other.Y, Z * Other.Z);
	}

	friend bool operator==(FIntegerVector const& FirstArg, FIntegerVector const& Other)
	{
		return (FirstArg.X == Other.X && FirstArg.Y == Other.Y && FirstArg.Z == Other.Z);
	}
};

USTRUCT(BlueprintType)
struct FWallAndIndex
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* Mesh = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int Index = 0;

	FWallAndIndex() {
		Mesh = nullptr;
		Index = 0;
	};

	FWallAndIndex(UStaticMeshComponent* MeshParam, int IndexParam) {
		Mesh = MeshParam;
		Index = IndexParam;
	};
};

UCLASS()
class PARTYCRASH_API ARoom : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ARoom();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<FRoomCell> CellsArray;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FIntegerVector RoomBoundsSize;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FIntegerVector RootCellLocation;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<bool> UsedCells;

	/** Please add a variable description */
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TSoftObjectPtr<UWorld> LevelToLoad;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	ULevelStreamingDynamic* Level;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FTransform RelativeSpawnLevelTransform;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int CurrentFacing;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<WorldDir> WindowsDirections;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UMaterialInterface* WallMaterial;
	
	UFUNCTION(BlueprintCallable)
	FIntegerVector CalculateCellLocation(int CellIndex);

	UFUNCTION(BlueprintCallable)
	FIntegerVector RotateCellLocation(FIntegerVector OriginalCellLocation);

	UFUNCTION(BlueprintCallable)
	int ConvertCellLocationToIndex(FIntegerVector CellLocation);

	UFUNCTION(BlueprintCallable)
	bool CheckIsLocationTaken(FIntegerVector CellLocation);

	UFUNCTION(BlueprintCallable)
	void RotateRoomRight();

	UFUNCTION(BlueprintCallable)
	void ClearDoublingWalls(FIntegerVector LocationToClear, TArray<WorldDir> DirectionsToClear);

	UFUNCTION(BlueprintCallable)
	void SetToDesiredFacing(int DesiredFacing);

	UFUNCTION(BlueprintCallable)
	TArray<FWallAndIndex> FindWallsRefsAndMatIndexes(int CellIndex, WorldDir Direction, bool Inside);

	//UFUNCTION(BlueprintCallable)
	TArray<FWallAndIndex> FindWallsRefsAndMatIndexes(FRoomCell CellParam, WorldDir Direction, bool Inside);

	UFUNCTION(BlueprintCallable)
	int CalculateMatIndex(UStaticMeshComponent* Mesh, WorldDir Direction, bool Inside);

	UFUNCTION(BlueprintCallable)
	void PaintInsideWalls();

	UFUNCTION(BlueprintCallable)
	void PaintOutsideWalls(UMaterialInterface* MatParam, FRoomCell CellParam, WorldDir Direction);

	/*UFUNCTION(BlueprintCallable)
	void PaintOutsideWalls(UMaterialInterface* MatParam, FRoomCell CellParam, WorldDir Direction);

	UFUNCTION(BlueprintCallable)
	void PaintOutsideWalls(UMaterialInterface* MatParam, FRoomCell CellParam, WorldDir Direction);*/

	UFUNCTION(BlueprintCallable)
	void FillCellsInfo();
};
