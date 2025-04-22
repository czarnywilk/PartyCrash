// Fill out your copyright notice in the Description page of Project Settings.


#include "WorldBuilder.h"

// Sets default values
AWorldBuilder::AWorldBuilder()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AWorldBuilder::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AWorldBuilder::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AWorldBuilder::BuildNewMap()
{
	int XGridSize = GridSize.X;
	int YGridSize = GridSize.Y;
	int ZGridSize = GridSize.Z;

	GridCells.Empty();
	GridCells.Init(FRoomCell(), XGridSize * YGridSize * ZGridSize);

	RoomsRefArray.Empty();
	RoomsRefArray.Init(nullptr, XGridSize * YGridSize * ZGridSize);

	//MainRoom->ClearDoublingWalls(FIntegerVector(0, 0, 0), { WorldDir::NORTH });
	TakenCells.Add(MainRoomLocation);
	SetRoomOnPosition(MainRoom, MainRoomLocation);

	for (int Z = 0; Z < ZGridSize; Z++) {
		for (int Y = 0; Y < YGridSize; Y++) {
			for (int X = 0; X < XGridSize; X++) {
				FIntegerVector Temp = FIntegerVector(X, Y, Z);
				if (TakenCells.Contains(Temp))
					continue;
				TArray<FRoomAndFacing> ArrayOfGoodClassesAndFacings = GetRoomsThatCanFitLocation(Temp);
				int RandomIndex = FMath::RandRange(0, ArrayOfGoodClassesAndFacings.Num() - 1);
				if (ArrayOfGoodClassesAndFacings.IsEmpty()) {
					GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::White, "No good rooms!");
					continue;
				}
				FRoomAndFacing RandomRoomSetting = ArrayOfGoodClassesAndFacings[RandomIndex];

				ARoom* AnotherRoom = GetWorld()->SpawnActorDeferred<ARoom>(RandomRoomSetting.RoomClass, FTransform(CalculateRoomLocation(Temp)));
				if (AnotherRoom->IsValidLowLevelFast()) {
					AnotherRoom->FinishSpawning(FTransform());
					AnotherRoom->SetToDesiredFacing(RandomRoomSetting.Facing);
					SetRoomOnPosition(AnotherRoom, Temp);

				}
			}
		}
	}
}

FVector AWorldBuilder::CalculateRoomLocation(FIntegerVector GridPosition)
{
	FIntegerVector Difference = MainRoomLocation - GridPosition;
	FVector DifferenceLoc = FVector(Difference.X * 800.0f, Difference.Y * 800.0f, Difference.Z * 370.0f);
	return MainRoom->GetActorLocation() - DifferenceLoc;
}

int AWorldBuilder::CalculateIndex(FIntegerVector Location)
{
	int XGridSize = GridSize.X;
	int YGridSize = GridSize.Y;
	int ZGridSize = GridSize.Z;
	return Location.Z * XGridSize * YGridSize + Location.Y * XGridSize + Location.X;
}

bool AWorldBuilder::CanRoomFitIntoLocation(ARoom* Room, FIntegerVector Location)
{
	FIntegerVector RoomEndGlobalPoint = Location + Room->RoomBoundsSize + FIntegerVector(-1,-1,-1);
	if (RoomEndGlobalPoint.X >= GridSize.X || RoomEndGlobalPoint.Y >= GridSize.Y || RoomEndGlobalPoint.Z >= GridSize.Z)
		return false;
	if (!CheckWindowCompability(Room, Location)) return false;

	for (int Z = 0; Z < Room->RoomBoundsSize.Z; Z++) {
		for (int Y = 0; Y < Room->RoomBoundsSize.Y; Y++) {
			for (int X = 0; X < Room->RoomBoundsSize.X; X++) {
				FIntegerVector CurrentVec = FIntegerVector(X, Y, Z);
				if (Room->CheckIsLocationTaken(CurrentVec)) {
					FRoomCell CellToCheck = Room->CellsArray[Room->ConvertCellLocationToIndex(CurrentVec)];
					FIntegerVector GlobalLocation = CurrentVec + Location;
					if (!CheckLocationIsInGrid(GlobalLocation)) return false;
					if (!CanCellFitIntoLocation(CellToCheck, GlobalLocation)) return false;
				}
			}
		}
	}

	return true;
}

bool AWorldBuilder::CheckDoorCompability(FRoomCell FirstCell, FIntegerVector FirstLocation, FRoomCell SecondCell, FIntegerVector SecondLocation)
{
	if (FirstLocation - SecondLocation == FIntegerVector(-1, 0, 0)) {
		return (FirstCell.DoorNorth + SecondCell.DoorSouth == 5 || FirstCell.DoorNorth + SecondCell.DoorSouth == 0);
	}
	else if (FirstLocation - SecondLocation == FIntegerVector(1, 0, 0)) {
		return (FirstCell.DoorSouth + SecondCell.DoorNorth == 5 || FirstCell.DoorSouth + SecondCell.DoorNorth == 0);
	}
	else if (FirstLocation - SecondLocation == FIntegerVector(0, 1, 0)) {
		return (FirstCell.DoorWest + SecondCell.DoorEast == 5 || FirstCell.DoorWest + SecondCell.DoorEast == 0);
	}
	else if (FirstLocation - SecondLocation == FIntegerVector(0, -1, 0)) {
		return (FirstCell.DoorEast + SecondCell.DoorWest == 5 || FirstCell.DoorEast + SecondCell.DoorWest == 0);
	}
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::White, "Sprawdzanie drzwi");
	return true;
}

bool AWorldBuilder::CheckWindowCompability(ARoom* Room, FIntegerVector Location)
{
	FIntegerVector EndLocation = Location + Room->RoomBoundsSize + FIntegerVector(-1, -1, -1);
	TArray<WorldDir> TempArray;
	if (!CheckLocationIsInGrid(Location + FIntegerVector(-1, 0, 0))) TempArray.Add(WorldDir::SOUTH);
	if (!CheckLocationIsInGrid(Location + FIntegerVector(0, -1, 0))) TempArray.Add(WorldDir::WEST);
	if (!CheckLocationIsInGrid(EndLocation + FIntegerVector(1, 0, 0))) TempArray.Add(WorldDir::NORTH);
	if (!CheckLocationIsInGrid(EndLocation + FIntegerVector(0, 1, 0))) TempArray.Add(WorldDir::EAST);

	if (Room->WindowsDirections.Num() != TempArray.Num()) return false;

	for (WorldDir Direction : Room->WindowsDirections) {
		if (!TempArray.Contains(Direction)) return false;
	}

	return true;
}

bool AWorldBuilder::CanCellFitIntoLocation(FRoomCell Cell, FIntegerVector Location)
{
	if (TakenCells.Contains(Location)) return false;

	int XLoc = Location.X;
	int YLoc = Location.Y;
	
	for (int y = -1; y <= 1; y++) {
		for (int x = -1; x <= 1; x++) {
			if (abs(x + y) != 1) continue;
			FIntegerVector VecToCheck = FIntegerVector(XLoc + x, YLoc + y, Location.Z);
			if (CheckLocationIsInGrid(VecToCheck) && TakenCells.Contains(VecToCheck)) {
				if (!CheckDoorCompability(Cell, Location, GridCells[CalculateIndex(VecToCheck)], VecToCheck)) return false;
			}
		}
	}
	return true;
}

bool AWorldBuilder::CheckLocationIsInGrid(FIntegerVector Location)
{
	return (Location.Z >= 0 && Location.Y >= 0 && Location.X >= 0 && Location.Z < GridSize.Z && Location.Y < GridSize.Y && Location.X < GridSize.X);
}

TArray<FRoomAndFacing> AWorldBuilder::GetRoomsThatCanFitLocation(FIntegerVector Location)
{
	TArray<FRoomAndFacing> CurrentArray;
	//ARoom* Room;
	for (TSubclassOf<ARoom> RoomClass : MainRoomClasses) {
		ARoom* Room = GetWorld()->SpawnActorDeferred<ARoom>(RoomClass, FTransform());
		for (int i = 0; i < 4; i++) {
			if (CanRoomFitIntoLocation(Room, Location))
				int NewIndex = CurrentArray.Add(FRoomAndFacing(RoomClass, i));
			Room->RotateRoomRight();
		}
		Room->Destroy();
	}

	return CurrentArray;
}

void AWorldBuilder::SetRoomOnPosition(ARoom* RoomParam, FIntegerVector Location)
{
	FIntegerVector FinalLocation = Location + RoomParam->RootCellLocation;
	RoomParam->SetActorLocation(CalculateRoomLocation(FinalLocation));
	//RoomParam->SetActorLocation(CalculateRoomLocation(Location));

	int ZGridSize = RoomParam->RoomBoundsSize.Z;
	int YGridSize = RoomParam->RoomBoundsSize.Y;
	int XGridSize = RoomParam->RoomBoundsSize.X;

	for (int Z = 0; Z < ZGridSize; Z++) {
		for (int Y = 0; Y < YGridSize; Y++) {
			for (int X = 0; X < XGridSize; X++) {
				FIntegerVector CurrentLocalVec = FIntegerVector(X, Y, Z);
				if (RoomParam->CheckIsLocationTaken(CurrentLocalVec)) {
					
					//Write new taken cells
					FRoomCell CurrentCell = RoomParam->CellsArray[RoomParam->ConvertCellLocationToIndex(CurrentLocalVec)];
					FIntegerVector CurrentGlobalVec = Location + CurrentLocalVec;
					
					RoomParam->Tags.Add(FName(CurrentGlobalVec.ConvertToName()));
					RoomsRefArray[CalculateIndex(CurrentGlobalVec)] = RoomParam;
					
					if (!CheckLocationIsInGrid(CurrentGlobalVec)) continue;
					GridCells[CalculateIndex(CurrentGlobalVec)] = CurrentCell;
					TakenCells.Add(CurrentGlobalVec);

					//Remove doubling walls
					TArray<WorldDir> DirectionsToClear;
					if (TakenCells.Contains(FIntegerVector(1, 0, 0) + CurrentGlobalVec) && CheckLocationIsInGrid(FIntegerVector(1, 0, 0) + CurrentGlobalVec)) {
						DirectionsToClear.Add(WorldDir::NORTH);
						ARoom* NeighborRoom = RoomsRefArray[CalculateIndex(FIntegerVector(1, 0, 0) + CurrentGlobalVec)];
						FRoomCell NeighborCell = GridCells[CalculateIndex(FIntegerVector(1, 0, 0) + CurrentGlobalVec)];
						NeighborRoom->PaintOutsideWalls(RoomParam->WallMaterial, NeighborCell, WorldDir::SOUTH);
					}
					if (TakenCells.Contains(FIntegerVector(-1, 0, 0) + CurrentGlobalVec) && CheckLocationIsInGrid(FIntegerVector(-1, 0, 0) + CurrentGlobalVec)) {
						DirectionsToClear.Add(WorldDir::SOUTH);
						ARoom* NeighborRoom = RoomsRefArray[CalculateIndex(FIntegerVector(-1, 0, 0) + CurrentGlobalVec)];
						FRoomCell NeighborCell = GridCells[CalculateIndex(FIntegerVector(-1, 0, 0) + CurrentGlobalVec)];
						NeighborRoom->PaintOutsideWalls(RoomParam->WallMaterial, NeighborCell, WorldDir::NORTH);
					}
					if (TakenCells.Contains(FIntegerVector(0, 1, 0) + CurrentGlobalVec) && CheckLocationIsInGrid(FIntegerVector(0, 1, 0) + CurrentGlobalVec)) {
						DirectionsToClear.Add(WorldDir::EAST);
						ARoom* NeighborRoom = RoomsRefArray[CalculateIndex(FIntegerVector(0, 1, 0) + CurrentGlobalVec)];
						FRoomCell NeighborCell = GridCells[CalculateIndex(FIntegerVector(0, 1, 0) + CurrentGlobalVec)];
						NeighborRoom->PaintOutsideWalls(RoomParam->WallMaterial, NeighborCell, WorldDir::WEST);
					}
					if (TakenCells.Contains(FIntegerVector(0, -1, 0) + CurrentGlobalVec) && CheckLocationIsInGrid(FIntegerVector(0, -1, 0) + CurrentGlobalVec)) {
						DirectionsToClear.Add(WorldDir::WEST);
						ARoom* NeighborRoom = RoomsRefArray[CalculateIndex(FIntegerVector(0, -1, 0) + CurrentGlobalVec)];
						FRoomCell NeighborCell = GridCells[CalculateIndex(FIntegerVector(0, -1, 0) + CurrentGlobalVec)];
						NeighborRoom->PaintOutsideWalls(RoomParam->WallMaterial, NeighborCell, WorldDir::EAST);
					}
					RoomParam->ClearDoublingWalls(CurrentLocalVec, DirectionsToClear);
				}
			}
		}
	}
}

