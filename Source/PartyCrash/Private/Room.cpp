// Fill out your copyright notice in the Description page of Project Settings.


#include "Room.h"
#include "Math/Int128.h"


// Sets default values
ARoom::ARoom()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	CurrentFacing = 0;
	SetActorTickEnabled(false);

}

// Called when the game starts or when spawned
void ARoom::BeginPlay()
{
	Super::BeginPlay();
	UChildActorComponent* MiddlePoint = Cast<UChildActorComponent>(GetDefaultSubobjectByName("MiddlePoint"));
	if (MiddlePoint->IsValidLowLevelFast()) {
		FVector RelativeLoc = MiddlePoint->GetRelativeLocation();

		TArray<USceneComponent*> StaticComps;
		GetComponents<USceneComponent>(StaticComps);

		for (USceneComponent* TempComp : StaticComps) {
			if (TempComp->GetAttachParent() == GetRootComponent() && TempComp != GetRootComponent()) {
				TempComp->SetRelativeLocation(TempComp->GetRelativeLocation() - RelativeLoc);
			}
		}
	}

	FillCellsInfo();

	FVector Location = RelativeSpawnLevelTransform.GetLocation() + GetActorLocation();
	FRotator Rotation = FRotator(RelativeSpawnLevelTransform.GetRotation()) + GetActorRotation();
	bool Succeded;
	Level = ULevelStreamingDynamic::LoadLevelInstanceBySoftObjectPtr(this, LevelToLoad, Location, Rotation, Succeded);

	PaintInsideWalls();
}

// Called every frame
void ARoom::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

FIntegerVector ARoom::CalculateCellLocation(int CellIndex)
{
	int FinalIndex = 0;
	for (int Z = 0; Z < RoomBoundsSize.Z; Z++) {
		for (int Y = 0; Y < RoomBoundsSize.Y; Y++) {
			for (int X = 0; X < RoomBoundsSize.X; X++) {
				int CurrentIndex = Z * RoomBoundsSize.Y * RoomBoundsSize.X + Y * RoomBoundsSize.X + X;
				if (FinalIndex == CellIndex && UsedCells[CurrentIndex]) {
					return FIntegerVector(X, Y, Z);
				}
				else if (UsedCells[CurrentIndex]) {
					FinalIndex++;
				}
			}
		}
	}
	return FIntegerVector(-1,-1,-1);
}

int ARoom::ConvertCellLocationToIndex(FIntegerVector CellLocation)
{
	int PrimaryIndex = CellLocation.Z * RoomBoundsSize.Y * RoomBoundsSize.X + CellLocation.Y * RoomBoundsSize.X + CellLocation.X;
	int FinalIndex = -1;
	for (int i = 0; i < PrimaryIndex+1; i++){
		if (UsedCells[i]) FinalIndex++;
	}
	return FinalIndex;
}

bool ARoom::CheckIsLocationTaken(FIntegerVector CellLocation)
{
	if (CellLocation.X < 0 || CellLocation.X >= RoomBoundsSize.X) return false;
	if (CellLocation.Y < 0 || CellLocation.Y >= RoomBoundsSize.Y) return false;
	if (CellLocation.Z < 0 || CellLocation.Z >= RoomBoundsSize.Z) return false;
	int PrimaryIndex = CellLocation.Z * RoomBoundsSize.Y * RoomBoundsSize.X + CellLocation.Y * RoomBoundsSize.X + CellLocation.X;
	return UsedCells[PrimaryIndex];
}

FIntegerVector ARoom::RotateCellLocation(FIntegerVector OriginalCellLocation)
{

	int X = OriginalCellLocation.X;
	int Y = OriginalCellLocation.Y;
	int Z = OriginalCellLocation.Z;
	int Temp;

	/*Temp = X;
	X = Y;
	Y = -Temp;*/

	Temp = Y;
	Y = X;
	X = -Temp;

	return FIntegerVector(X, Y, Z);
}

void ARoom::RotateRoomRight()
{
	//Create Array of Rotated Cells Locations
	TArray<FIntegerVector> CellsLocations;
	FIntegerVector MinCellLocation(0, 0, 0);
	for (int i = 0; i < CellsArray.Num(); i++) {
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, "NORMAL CELL ");
		CellsLocations.Add(RotateCellLocation(CalculateCellLocation(i)));
		if (CellsLocations[i].X < MinCellLocation.X) MinCellLocation.X = CellsLocations[i].X;
		if (CellsLocations[i].Y < MinCellLocation.Y) MinCellLocation.Y = CellsLocations[i].Y;
	}

	UE_LOG(LogTemp, Warning, TEXT("Jeden"));

	//Calculate every empty cell location in rotated room and rotate it
	TArray<FIntegerVector> EmptyCells;
	for (int Z = 0; Z < RoomBoundsSize.Z; Z++) {
		for (int Y = 0; Y < RoomBoundsSize.Y; Y++) {
			for (int X = 0; X < RoomBoundsSize.X; X++) {
				int CurrentIndex = Z * RoomBoundsSize.Y * RoomBoundsSize.X + Y * RoomBoundsSize.X + X;
				if (!UsedCells.IsValidIndex(CurrentIndex) || !UsedCells[CurrentIndex]) {
					GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, "EMPTY CELL ");
					EmptyCells.Add(RotateCellLocation(FIntegerVector(X, Y, Z)));
					if (EmptyCells.Top().X < MinCellLocation.X) MinCellLocation.X = EmptyCells.Top().X;
					if (EmptyCells.Top().Y < MinCellLocation.Y) MinCellLocation.Y = EmptyCells.Top().Y;
				}
			}
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("Dwa"));


	//Calculate new root location and adjust other cells locations
	RootCellLocation = RotateCellLocation(RootCellLocation) - MinCellLocation;
	for (int i = 0; i < CellsLocations.Num(); i++) {
		CellsLocations[i] = CellsLocations[i] - MinCellLocation;
	}
	for (int i = 0; i < EmptyCells.Num(); i++) {
		EmptyCells[i] = EmptyCells[i] - MinCellLocation;
	}

	UE_LOG(LogTemp, Warning, TEXT("Trzy"));

	//Changing Room Dimensions
	int TempXSize = RoomBoundsSize.X;
	RoomBoundsSize.X = RoomBoundsSize.Y;
	RoomBoundsSize.Y = TempXSize;

	//Recreating UsedCells Array
	for (int Z = 0; Z < RoomBoundsSize.Z; Z++) {
		for (int Y = 0; Y < RoomBoundsSize.Y; Y++) {
			for (int X = 0; X < RoomBoundsSize.X; X++) {
				int CurrentIndex = Z * RoomBoundsSize.Y * RoomBoundsSize.X + Y * RoomBoundsSize.X + X;
				if (CellsLocations.Contains<FIntegerVector>(FIntegerVector(X, Y, Z))) {
					//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::FromHex("#96db27"), FIntegerVector(X, Y, Z).ConvertToName());
					UsedCells[CurrentIndex] = true;
				}
				else {
					//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::FromHex("#d1566c"), FIntegerVector(X, Y, Z).ConvertToName());
					UsedCells[CurrentIndex] = false;
				}
			}
		}
	}
	UE_LOG(LogTemp, Warning, TEXT("Cztery"));

	//Creating Temp Cells Array
	TArray<FRoomCell> TempCellArray = CellsArray;
	for (int i = 0; i < CellsLocations.Num(); i++) {
		TempCellArray[ConvertCellLocationToIndex(CellsLocations[i])] = CellsArray[i];
	}
	CellsArray = TempCellArray;

	for (int i = 0; i < CellsArray.Num(); i++) {
		int Index = ConvertCellLocationToIndex(CellsLocations[i]);
		//TempCellArray[Index] = CellsArray[i];
		if (CellsArray.IsValidIndex(Index)) 
			CellsArray[Index].RotateCellRight();
	}

	CurrentFacing = CurrentFacing + 1;
	while (CurrentFacing < 0 || CurrentFacing > 3) CurrentFacing = CurrentFacing - FMath::Sign(CurrentFacing) * 4;

	UE_LOG(LogTemp, Warning, TEXT("Piec"));


	for (int i = 0; i < WindowsDirections.Num(); i++) {
		switch (WindowsDirections[i]) {
		case WorldDir::NORTH :
			WindowsDirections[i] = WorldDir::EAST;
			break;
		case WorldDir::EAST :
			WindowsDirections[i] = WorldDir::SOUTH;
			break;
		case WorldDir::SOUTH:
			WindowsDirections[i] = WorldDir::WEST;
			break;
		case WorldDir::WEST:
			WindowsDirections[i] = WorldDir::NORTH;
			break;
		}

	}
}

void ARoom::ClearDoublingWalls(FIntegerVector LocationToClear, TArray<WorldDir> DirectionsToClear) {
	
	int Index = ConvertCellLocationToIndex(LocationToClear);
	FRoomCell* CellToClear = &CellsArray[Index];

	for (WorldDir CurrentDir : DirectionsToClear) {
		TArray<FName> TempNames;
		if (CurrentDir == WorldDir::NORTH) TempNames = CellToClear->NorthComps;
		else if (CurrentDir == WorldDir::EAST) TempNames = CellToClear->EastComps;
		else if (CurrentDir == WorldDir::SOUTH) TempNames = CellToClear->SouthComps;
		else TempNames = CellToClear->WestComps;
		for (FName Name : TempNames) {
			USceneComponent* CurrentComp = Cast<USceneComponent>(GetDefaultSubobjectByName(Name));
			if (!CurrentComp->IsValidLowLevelFast()) {
				FString Message;
				Message = this->GetClass()->GetName() + " - " + Name.ToString();
				//Message->Append(" - " + Name.ToString());
				UE_LOG(LogTemp, Warning, TEXT("Message: %s"), *FString(Message));
				continue;
			}
			CurrentComp->DestroyComponent();
		}
		if (CurrentDir == WorldDir::NORTH) CellToClear->NorthComps.Empty();
		else if (CurrentDir == WorldDir::EAST) CellToClear->EastComps.Empty();
		else if (CurrentDir == WorldDir::SOUTH) CellToClear->SouthComps.Empty();
		else CellToClear->WestComps.Empty();
	}
}

void ARoom::SetToDesiredFacing(int DesiredFacing)
{
	int TimesToRotate = DesiredFacing - CurrentFacing;
	while (TimesToRotate < 0 || TimesToRotate > 3) TimesToRotate = TimesToRotate - FMath::Sign(TimesToRotate) * 4;
	for (int i = 0; i < TimesToRotate; i++) {
		RotateRoomRight();
	}
	SetActorRotation(FRotator(0, DesiredFacing * 90.0f, 0));
	//CurrentFacing = DesiredFacing;
}

TArray<FWallAndIndex> ARoom::FindWallsRefsAndMatIndexes(int CellIndex, WorldDir Direction, bool Inside) {
	
	TArray<FWallAndIndex> TempArray;

	if (!CellsArray.IsValidIndex(CellIndex)) return TempArray;

	return FindWallsRefsAndMatIndexes(CellsArray[CellIndex], Direction, Inside);
}

TArray<FWallAndIndex> ARoom::FindWallsRefsAndMatIndexes(FRoomCell CellParam, WorldDir Direction, bool Inside) {

	TArray<FName> NamesArray;
	TArray<FWallAndIndex> TempArray;

	switch (Direction) {
	case WorldDir::NORTH:
		NamesArray = CellParam.NorthComps;
		break;
	case WorldDir::EAST:
		NamesArray = CellParam.EastComps;
		break;
	case WorldDir::SOUTH:
		NamesArray = CellParam.SouthComps;
		break;
	case WorldDir::WEST:
		NamesArray = CellParam.WestComps;
		break;
	}

	for (FName Temp : NamesArray) {
		UStaticMeshComponent* CurrentComp = Cast<UStaticMeshComponent>(GetDefaultSubobjectByName(Temp));
		if (CurrentComp->IsValidLowLevelFast()) {
			if (CurrentComp->GetStaticMesh()->GetFName().ToString().Contains("wall")) {
				TempArray.Add(FWallAndIndex(CurrentComp, CalculateMatIndex(CurrentComp, Direction, Inside)));
			}
		}
	}

	return TempArray;
}

int ARoom::CalculateMatIndex(UStaticMeshComponent* Mesh, WorldDir Direction, bool Inside) {
	
	int North = 0;
	int East = 0;
	int South = 2;
	int West = 2;

	int RoundedLocation = 0;
	float Yaw = Mesh->GetComponentRotation().Yaw;

	if (FMath::IsWithin(Yaw, 179.0, 181.0)) RoundedLocation = 2;
	else if (FMath::IsWithin(Yaw, 89.0, 91.0)) RoundedLocation = 1;
	else if (FMath::IsWithin(Yaw, 269.0, 271.0)) RoundedLocation = 3;
	else if (FMath::IsWithin(Yaw, -181.0, -179.0)) RoundedLocation = 2;
	else if (FMath::IsWithin(Yaw, -271.0, -269.0)) RoundedLocation = 1;
	else if (FMath::IsWithin(Yaw, -91.0, -89.0)) RoundedLocation = 3;

	if (Direction == WorldDir::NORTH) {
		if (RoundedLocation == 0) {
			return Inside ? North : South;
		}
		else if (RoundedLocation == 2) {
			return Inside ? South : North;
		}
	}
	else if (Direction == WorldDir::SOUTH) {
		if (RoundedLocation == 0) {
			return Inside ? South : North;
		}
		else if (RoundedLocation == 2) {
			return Inside ? North : South;
		}
	}
	else if (Direction == WorldDir::EAST) {
		if (RoundedLocation == 1) {
			return Inside ? East : West;
		}
		else if (RoundedLocation == 3) {
			return Inside ? West : East;
		}
	}
	else if (Direction == WorldDir::WEST) {
		if (RoundedLocation == 1) {
			return Inside ? West : East;
		}
		else if (RoundedLocation == 3) {
			return Inside ? East : West;
		}
	}
	
	return 0;
}

void ARoom::PaintInsideWalls() {

	if (!WallMaterial->IsValidLowLevelFast()) return;
	TArray<WorldDir> TempArray = { WorldDir::NORTH, WorldDir::EAST, WorldDir::SOUTH, WorldDir::WEST };
	for (int Cell = 0; Cell < CellsArray.Num(); Cell++) {
		for (WorldDir Direction : TempArray) {
			for (FWallAndIndex Element : FindWallsRefsAndMatIndexes(Cell, Direction, true)) {
				Element.Mesh->SetMaterial(Element.Index, WallMaterial);
			}
		}
	}
}

void ARoom::PaintOutsideWalls(UMaterialInterface* MatParam, FRoomCell CellParam, WorldDir Direction) {
	if (!MatParam->IsValidLowLevelFast()) return;
	
	for (FWallAndIndex Element : FindWallsRefsAndMatIndexes(CellParam, Direction, false)) {
		Element.Mesh->SetMaterial(Element.Index, MatParam);
	}
}

void ARoom::FillCellsInfo()
{

	//Creating array of wall static meshes
	TArray<UStaticMeshComponent*> StaticMeshComps;
	GetComponents<UStaticMeshComponent>(StaticMeshComps);

	for (int i = StaticMeshComps.Num() - 1; i >= 0; i-- ) {
		if (!StaticMeshComps[i]->GetStaticMesh()->GetName().Contains("wall"))
			StaticMeshComps.RemoveAt(i);
	}

	//Creating array of shared roots

	TArray<USceneComponent*> SharedRootComps;
	GetComponents<USceneComponent>(SharedRootComps);

	for (int i = SharedRootComps.Num() - 1; i >= 0; i--) {
		if (!SharedRootComps[i]->GetName().Contains("sharedroot"))
			SharedRootComps.RemoveAt(i);
	}

	//Proceed...

	int CurrentIndex = 0;
	for (int i = 0; i < UsedCells.Num(); i++) {
		if (UsedCells[i]) {
			if (CurrentIndex == 0)
				CellsArray[CurrentIndex].CellName = "Root";
			else CellsArray[CurrentIndex].CellName = "Cell" + FString::FromInt(CurrentIndex - 1);

			CellsArray[CurrentIndex].DoorEast = 0;
			CellsArray[CurrentIndex].DoorNorth = 0;
			CellsArray[CurrentIndex].DoorSouth = 0;
			CellsArray[CurrentIndex].DoorWest = 0;

			CellsArray[CurrentIndex].EastComps.Empty();
			CellsArray[CurrentIndex].NorthComps.Empty();
			CellsArray[CurrentIndex].SouthComps.Empty();
			CellsArray[CurrentIndex].WestComps.Empty();

			FVector CellLocation = FIntegerVector(CalculateCellLocation(CurrentIndex) * FIntegerVector(800, 800, 370)).ToFloatVector();

			for (int Index = StaticMeshComps.Num() - 1; Index >= 0; Index--) {
				FVector Difference = StaticMeshComps[Index]->GetRelativeLocation() - CellLocation;
				FString Logging = Difference.ToString();
				UE_LOG(LogTemp, Warning, TEXT("Difference: %s"), *FString(Logging));

				float Yaw = StaticMeshComps[Index]->GetRelativeRotation().Yaw;
				bool IsTurnedWest = FMath::IsWithinInclusive(Yaw, 179.0, 181.0) || FMath::IsWithinInclusive(Yaw, -181.0, -179.0);
				bool IsTurnedSouth = FMath::IsWithinInclusive(Yaw, 269.0, 271.0) || FMath::IsWithinInclusive(Yaw, -91.0, -89.0);
				bool IsTurnedNorth = FMath::IsWithinInclusive(Yaw, 89.0, 91.0) || FMath::IsWithinInclusive(Yaw, -271.0, -269.0);
				bool IsTurnedEast = FMath::IsWithinInclusive(Yaw, -1.0, 1.0) || FMath::IsWithinInclusive(Yaw, 359.0, 361.0);

				IsTurnedSouth = true;
				IsTurnedEast = true;
				
				bool XPositiveExact = FMath::IsWithinInclusive(Difference.X, 409.0, 411.0) || FMath::IsWithinInclusive(Difference.X, 389.0, 391.0);
				bool XNegativeExact = FMath::IsWithinInclusive(Difference.X, -411.0, -409.0) || FMath::IsWithinInclusive(Difference.X, -391.0, -389.0);
				bool XInRange = Difference.X <= (IsTurnedNorth ? 390 : 410.0) && Difference.X >= (IsTurnedNorth ? -410.0 : -390.0);

				bool YPositiveExact = FMath::IsWithinInclusive(Difference.Y, 409.0, 411.0) || FMath::IsWithinInclusive(Difference.Y, 389.0, 391.0);
				bool YNegativeExact = FMath::IsWithinInclusive(Difference.Y, -411.0, -409.0) || FMath::IsWithinInclusive(Difference.Y, -391.0, -389.0);
				bool YInRange = Difference.Y <= (IsTurnedWest ? 390 : 410.0) && Difference.Y >= (IsTurnedWest ? -410.0 : -390.0);

				bool ZInRange = Difference.Z <= 10.0 && Difference.Z >= -10.0;

				bool IsNorth = XPositiveExact && YInRange && ZInRange && (IsTurnedWest || IsTurnedEast);
				bool IsSouth = XNegativeExact && YInRange && ZInRange && (IsTurnedWest || IsTurnedEast);
				bool IsWest = YNegativeExact && XInRange && ZInRange && (IsTurnedNorth || IsTurnedSouth);
				bool IsEast = YPositiveExact && XInRange && ZInRange && (IsTurnedNorth || IsTurnedSouth);

				if (IsNorth) {
					CellsArray[CurrentIndex].NorthComps.Add(FName(StaticMeshComps[Index]->GetName()));
					StaticMeshComps.RemoveAt(Index);
				} else if (IsSouth) {
					CellsArray[CurrentIndex].SouthComps.Add(FName(StaticMeshComps[Index]->GetName()));
					StaticMeshComps.RemoveAt(Index);
				} else if (IsWest) {
					CellsArray[CurrentIndex].WestComps.Add(FName(StaticMeshComps[Index]->GetName()));
					StaticMeshComps.RemoveAt(Index);
				} else if (IsEast) {
					CellsArray[CurrentIndex].EastComps.Add(FName(StaticMeshComps[Index]->GetName()));
					StaticMeshComps.RemoveAt(Index);
				}
			}

			for (int Index = SharedRootComps.Num() - 1; Index >= 0; Index--) {

				FVector Difference = SharedRootComps[Index]->GetRelativeLocation() - CellLocation;

				float Yaw = SharedRootComps[Index]->GetRelativeRotation().Yaw;
				bool IsTurnedWest = FMath::IsWithinInclusive(Yaw, 179.0, 181.0) || FMath::IsWithinInclusive(Yaw, -181.0, -179.0);
				bool IsTurnedSouth = FMath::IsWithinInclusive(Yaw, 269.0, 271.0) || FMath::IsWithinInclusive(Yaw, -91.0, -89.0);
				bool IsTurnedNorth = FMath::IsWithinInclusive(Yaw, 89.0, 91.0) || FMath::IsWithinInclusive(Yaw, -271.0, -269.0);
				bool IsTurnedEast = FMath::IsWithinInclusive(Yaw, -1.0, 1.0) || FMath::IsWithinInclusive(Yaw, 359.0, 361.0);

				IsTurnedSouth = true;
				IsTurnedEast = true;

				bool XPositiveExact = FMath::IsWithinInclusive(Difference.X, 409.0, 411.0) || FMath::IsWithinInclusive(Difference.X, 389.0, 391.0);
				bool XNegativeExact = FMath::IsWithinInclusive(Difference.X, -411.0, -409.0) || FMath::IsWithinInclusive(Difference.X, -391.0, -389.0);
				bool XInRange = Difference.X <= (IsTurnedNorth ? 390 : 410.0) && Difference.X >= (IsTurnedNorth ? -410.0 : -390.0);

				bool YPositiveExact = FMath::IsWithinInclusive(Difference.Y, 409.0, 411.0) || FMath::IsWithinInclusive(Difference.Y, 389.0, 391.0);
				bool YNegativeExact = FMath::IsWithinInclusive(Difference.Y, -411.0, -409.0) || FMath::IsWithinInclusive(Difference.Y, -391.0, -389.0);
				bool YInRange = Difference.Y <= (IsTurnedWest ? 390 : 410.0) && Difference.Y >= (IsTurnedWest ? -410.0 : -390.0);

				bool ZInRange = Difference.Z <= 10.0 && Difference.Z >= -10.0;
				bool IsWall = SharedRootComps[Index]->GetName().Contains("door");

				FString Logging = Difference.ToString();
				UE_LOG(LogTemp, Warning, TEXT("Difference root: %s"), *FString(Logging));

				TArray<USceneComponent*> RootComps;
				SharedRootComps[Index]->GetChildrenComponents(false, RootComps);
				TArray<FName> CompsNames;
				for (USceneComponent* Comp : RootComps)
					CompsNames.Add(FName(Comp->GetName()));

				double XDiff = Difference.X + 600.0;
				XDiff = FMath::RoundToDouble(XDiff/100) * 100;
				double YDiff = Difference.Y + 600.0;
				YDiff = FMath::RoundToDouble(YDiff/100) * 100;

				int XToAdd = IsTurnedNorth ? 0 : 1;
				int YToAdd = IsTurnedWest ? 0 : 1;

				bool IsNorth = XPositiveExact && YInRange && ZInRange && (IsTurnedWest || IsTurnedEast);
				bool IsSouth = XNegativeExact && YInRange && ZInRange && (IsTurnedWest || IsTurnedEast);
				bool IsWest = YNegativeExact && XInRange && ZInRange && (IsTurnedNorth || IsTurnedSouth);
				bool IsEast = YPositiveExact && XInRange && ZInRange && (IsTurnedNorth || IsTurnedSouth);

				if (IsNorth) {
					CellsArray[CurrentIndex].NorthComps.Append(CompsNames);

					if (IsWall)
						CellsArray[CurrentIndex].DoorNorth = FMath::RoundToInt(YDiff) / 200 - YToAdd;

					SharedRootComps.RemoveAt(Index);
				}
				else if (IsSouth) {
					CellsArray[CurrentIndex].SouthComps.Append(CompsNames);

					if (IsWall)
						CellsArray[CurrentIndex].DoorSouth = 5 - FMath::RoundToInt(YDiff) / 200 + YToAdd;

					SharedRootComps.RemoveAt(Index);
				}
				else if (IsWest) {
					CellsArray[CurrentIndex].WestComps.Append(CompsNames);

					if (IsWall)
						CellsArray[CurrentIndex].DoorWest = FMath::RoundToInt(XDiff) / 200 - XToAdd;

					SharedRootComps.RemoveAt(Index);
				}
				else if (IsEast) {
					CellsArray[CurrentIndex].EastComps.Append(CompsNames);

					if (IsWall)
						CellsArray[CurrentIndex].DoorEast = 5 - FMath::RoundToInt(XDiff) / 200 + XToAdd;

					SharedRootComps.RemoveAt(Index);
				}
			}

			CurrentIndex++;
		}
	}
}
