

#include "OmGGrid.h"
#include "OmGUtils.h"
#include "OmGObjectBase.h"

#include "Engine/Engine.h"



AOmGGrid::AOmGGrid()
{

	PrimaryActorTick.bCanEverTick = false;

	SceneComponent = CreateDefaultSubobject<USceneComponent>("RootComponent");
	SetRootComponent(SceneComponent);


}



void AOmGGrid::InitializeGrid(TMap<EZoneType, FZoneGameplayData>& zoness, TMap<FVector2D, FSlotData>& cellss)
{
	CheckMainParams();

	GenerateZoneData();

	zoness = ZoneData;
	cellss = Cells;

}



void AOmGGrid::RegenerateGrid()
{
	CurrentHealth = 0;

	for (auto& obj : Objects)
	{
		obj->DestroyObj();
	}

	Objects.Empty();

	FTimerHandle TimerToRegenerating;
	GetWorldTimerManager().SetTimer(TimerToRegenerating, this, &AOmGGrid::SpawnObjectsInGrid, 0.25f, false);
}


void AOmGGrid::CheckMainParams()
{
	Cells.Empty();
	ZoneData.Empty();


	MaxFieldStrength -= MaxFieldStrength % 3;
	Lumbagos = FMath::Min(Lumbagos, Columns - 1);
}



void AOmGGrid::GenerateZoneData()
{
	InitializeGridCells();

	TMap<EZoneType, int32> NumberOfZones;

	for (int32 k = ZonesSpawnData.Num() - 1; k >= 0; --k)
	{
		EZoneType ZoneType = ZonesSpawnData[k].Zone;

		if (int32* zoneNum = NumberOfZones.Find(ZoneType))
		{
			*zoneNum += 1;
		}
		else
		{
			NumberOfZones.Add(ZoneType, 1);
		}



		if (ZonesSpawnData[k].Size == -1)
		{
			for (auto& cell : Cells)
			{
				if (cell.Value.Zone == EZoneType::ZoneNone)
				{
					FPositionInGrid cellPos = FPositionInGrid{ int32(cell.Key.X), int32(cell.Key.Y) };

					AddCellInZone(cellPos, ZoneType);
				}
			}


		}
		else
		{
			int32 RowStartIndex = FMath::Max(ZonesSpawnData[k].StartPosition.Row - 1, 0);
			int32 RowEndIndex = (FMath::Min(ZonesSpawnData[k].EndPosition.Row, Rows)) - 1;

			int32 ColumnStartIndex = FMath::Max(ZonesSpawnData[k].StartPosition.Column - 1, 0);
			int32 ColumnEndIndex = FMath::Min(ZonesSpawnData[k].EndPosition.Column, Columns) - 1;

			for (int32 i = RowStartIndex; i <= RowEndIndex; ++i)
			{
				for (int32 j = ColumnStartIndex; j <= ColumnEndIndex; ++j)
				{
					const FPositionInGrid cellPos{ i,j };

					if (IsCellInZone(cellPos, FPositionInGrid{ RowStartIndex, ColumnStartIndex },
						FPositionInGrid{ RowEndIndex, ColumnEndIndex }, ZonesSpawnData[k].Size))
					{

						FSlotData* cellData = Cells.Find(FVector2D{ float(i), float(j) });
						if (cellData->Zone == EZoneType::ZoneNone)
						{
							AddCellInZone(cellPos, ZoneType);
						}
						else
						{
							if (FZoneGameplayData* zon = ZoneData.Find(cellData->Zone))
							{
								zon->Cells.Remove(cellPos);


							}

							AddCellInZone(cellPos, ZoneType);
						}

					}

				}
			}
		}
	}

	for (auto& zone : NumberOfZones)
	{
		if (zone.Value > 1)
		{
			ZoneData.Find(zone.Key)->Objects.IncreasePool(zone.Value - 1);
		}
	}

	SortZoneDataByPriority();

	VisualizeGrid();
}



void AOmGGrid::InitializeGridCells()
{

	for (int32 i = 0; i < Rows; ++i)
	{
		for (int32 j = 0; j < Columns; ++j)
		{
			Cells.Add(FVector2D{ float(i),float(j) }, FSlotData{});
		}
	}

}

bool AOmGGrid::IsCellInZone(FPositionInGrid CellPos, FPositionInGrid StartPos, FPositionInGrid EndPos, int32 ZoneSize) const
{
	return !(((CellPos.Row >= StartPos.Row + ZoneSize) && (CellPos.Row <= EndPos.Row - ZoneSize))
		&& ((CellPos.Column >= StartPos.Column + ZoneSize) && (CellPos.Column <= EndPos.Column - ZoneSize)));

}

void AOmGGrid::AddCellInZone(FPositionInGrid CellPos, EZoneType Zone)
{
	if (FZoneGameplayData* zone = ZoneData.Find(Zone))
	{
		zone->Cells.Add(CellPos);
	}
	else
	{
		if (ObjectPools.Num() == 0 || ZonePriorities.Num() == 0)
		{
			FString ErrorMes = "Bad Setup: Check ObjectsPools and ZonePriorities settings";
			GEngine->AddOnScreenDebugMessage(-1, 7.f, FColor::Red, ErrorMes);

			return;
		}


		if (FObjectPool* objPool = ObjectPools.Find(Zone))
		{
			if (int32* priority = ZonePriorities.Find(Zone))
			{
				ZoneData.Add(Zone, FZoneGameplayData{ TArray<FPositionInGrid>{CellPos}, *objPool, *priority });
			}
		}
		else
		{
			FString ErrorMes = "Bad Setup: Check ObjectsPools and ZonePriorities settings";
			GEngine->AddOnScreenDebugMessage(-1, 7.f, FColor::Red, ErrorMes);
		}
	}

	FVector2D cellPosi{ float(CellPos.Row), float(CellPos.Column) };

	if (FSlotData* cell = Cells.Find(cellPosi))
	{
		cell->Zone = Zone;
	}
}



void AOmGGrid::SortZoneDataByPriority()
{
	TArray<FZoneDataPair> zoneDatBackup;

	for (TPair<EZoneType, FZoneGameplayData>& it : ZoneData)
	{
		FZoneDataPair pair;
		pair.Zone = it.Key;
		pair.ZoneDat = it.Value;

		zoneDatBackup.Insert(pair, GetPriorityIndex(zoneDatBackup));
	}

	ZoneData.Empty();

	for (auto& it : zoneDatBackup)
	{
		ZoneData.Add(it.Zone, it.ZoneDat);
	}
}



int32 AOmGGrid::GetPriorityIndex(TArray<FZoneDataPair>& data) const
{
	int32 BestPrior = 999999;

	int32 index = 0;

	for (; index < data.Num(); ++index)
	{
		if (data[index].ZoneDat.Priority < BestPrior)
		{
			return index;
		}
	}

	return index;
}


void AOmGGrid::VisualizeGrid()
{
	for (auto& cell : Cells)
	{
		FPositionInGrid cellPos = FPositionInGrid{ int32(cell.Key.X), int32(cell.Key.Y) };

		VisualizeCell(cellPos, cell.Value.Zone);
	}
}







void AOmGGrid::BeginPlay()
{
	Super::BeginPlay();

	SetupRightHealthes();

	SpawnObjectsInGrid();

}


void AOmGGrid::SetupRightHealthes()
{
	if (ObjectPools.Num() > 0)
	{
		for (auto& pair : ObjectPools)
		{
			for (auto& object : pair.Value.ObjectsData)
			{
				object.HP -= object.HP % 3;
			}
		}
	}

	LoadData();
}




void AOmGGrid::SpawnObjectsInGrid()
{
	TMap<EZoneType, FZoneGameplayData> CurrentZoneData = ZoneData;
	TMap<FVector2D, FSlotData> CurrentCells = Cells;

	if (Lumbagos > 0)
	{
		GenerateLumbagos(CurrentZoneData, CurrentCells);
	}

	while (CurrentHealth != MaxFieldStrength && CurrentZoneData.Num() > 0)
	{
		SpawnObject_Execution(CurrentZoneData, CurrentCells);
	}

	GenerationFinished();

}



void AOmGGrid::GenerateLumbagos(TMap<EZoneType, FZoneGameplayData>& CurrentZoneData, TMap<FVector2D, FSlotData>& CurrentCells)
{
	int32 NumOfRemainingLumbagos = Lumbagos;

	TArray<int32> BannedVerticals;




	while (NumOfRemainingLumbagos > 0)
	{
		int32 vertical = FMath::RandRange(0, Columns - 1);

		if ((BannedVerticals.Num() == 0) || (!BannedVerticals.Contains(vertical)))
		{
			BannedVerticals.Add(vertical);
			--NumOfRemainingLumbagos;

			for (int32 i = 0; i < Rows; ++i)
			{
				BanCell(FPositionInGrid{ i, vertical }, CurrentZoneData, CurrentCells);
			}

		}
	}
}



void AOmGGrid::BanCell(FPositionInGrid CellPos, TMap<EZoneType, FZoneGameplayData>& CurrentZoneData, TMap<FVector2D, FSlotData>& CurrentCells, AOmGObjectBase* ObjRef)
{
	FVector2D cellP{ float(CellPos.Row), float(CellPos.Column) };

	if (FSlotData* slot = CurrentCells.Find(cellP))
	{
		if (!slot->bIsBusy)
		{
			slot->bIsBusy = true;
			slot->ObjectRef = ObjRef;
			if (FZoneGameplayData* zoneD = CurrentZoneData.Find(slot->Zone))
			{
				zoneD->Cells.Remove(CellPos);

				if (zoneD->Cells.Num() == 0)
				{
					CurrentZoneData.Remove(slot->Zone);
				}
			}

			if (!ObjRef)
			{
				VisualizeCellBanning(CellPos);
			}
		}

	}
}





void AOmGGrid::SpawnObject_Execution(TMap<EZoneType, FZoneGameplayData>& CurrentZoneData, TMap<FVector2D, FSlotData>& CurrentCells)
{
	TPair<FPositionInGrid, EZoneType> CellForSpawning = GetPointForSpawn(CurrentZoneData);

	UE_LOG(LogTemp, Error, TEXT("WE SELECT POS &&&   %i x %i     ZONE %i"), CellForSpawning.Key.Row, CellForSpawning.Key.Column, CellForSpawning.Value);

	AddObjectInGrid(CellForSpawning, CurrentZoneData, CurrentCells);

}

TPair<FPositionInGrid, EZoneType> AOmGGrid::GetPointForSpawn(TMap<EZoneType, FZoneGameplayData> CurrentZoneData) const
{
	if (CurrentZoneData.Num() > 1)
	{
		while (true)
		{
			for (auto& zone : CurrentZoneData)
			{
				const float* SpawnChance = ObjectsSpawnChance.Find(zone.Key);
				EZoneType zoneType = zone.Key;

				for (auto& cell : zone.Value.Cells)
				{
					if (FMath::RandRange(0.0f, 1.0f) <= (*SpawnChance))
					{
						TPair<FPositionInGrid, EZoneType> pos{ cell, zoneType };
						return pos;
					}
				}

			}
		}
	}
	else
	{
		for (auto& zone : CurrentZoneData)
		{
			FPositionInGrid cell = zone.Value.Cells[FMath::RandRange(0, zone.Value.Cells.Num() - 1)];
			TPair<FPositionInGrid, EZoneType> pos{ cell, zone.Key };
			return pos;
		}
	}

	return TPair<FPositionInGrid, EZoneType>();
}



void AOmGGrid::AddObjectInGrid(TPair<FPositionInGrid, EZoneType>& Cell, TMap<EZoneType, FZoneGameplayData>& CurrentZoneData, TMap<FVector2D, FSlotData>& CurrentCells)
{
	TArray<FSpawnObjectData> LocalObjectsPool;

	EObjectType SpawnedObjType;

	GetAllEnabledObjectsForCell(LocalObjectsPool, Cell, CurrentZoneData, CurrentCells);

	if (LocalObjectsPool.Num() > 1)
	{
		for (auto& ObjData : LocalObjectsPool)
		{
			if (FMath::RandRange(0, 100) <= ObjData.ObjectData.Weight)
			{
				SpawnedObjType = ObjData.ObjectData.Object;
				auto ObjSpawnPos = ObjData.EnabledPositions.Cells[FMath::RandRange(0, ObjData.EnabledPositions.Cells.Num() - 1)];
				ProvideObjectSpawning(ObjData.ObjectData, ObjSpawnPos.Map, Cell.Value, CurrentZoneData, CurrentCells);

				break;
			}
		}
	}
	else
	{
		SpawnedObjType = LocalObjectsPool[0].ObjectData.Object;
		auto ObjSpawnPos = LocalObjectsPool[0].EnabledPositions.Cells[FMath::RandRange(0, LocalObjectsPool[0].EnabledPositions.Cells.Num() - 1)];
		ProvideObjectSpawning(LocalObjectsPool[0].ObjectData, ObjSpawnPos.Map, Cell.Value, CurrentZoneData, CurrentCells);
	}

	if (FZoneGameplayData* zoneD = CurrentZoneData.Find(Cell.Value))
	{
		int32 Index = 0;

		for (auto& objDat : zoneD->Objects.ObjectsData)
		{
			if (objDat.Object == SpawnedObjType)
			{
				if (objDat.Limit != -1)
				{
					objDat.Limit -= 1;

					if (objDat.Limit == 0)
					{
						zoneD->Objects.ObjectsData.RemoveAt(Index);
					}

					return;
				}

			}
			++Index;
		}
	}


}




void AOmGGrid::GetAllEnabledObjectsForCell(TArray<FSpawnObjectData>& ArrayToFill, TPair<FPositionInGrid, EZoneType>& Cell,
	TMap<EZoneType, FZoneGameplayData>& CurrentZoneData, TMap<FVector2D, FSlotData>& CurrentCells)
{

	for (auto& pool : CurrentZoneData.Find(Cell.Value)->Objects.ObjectsData)
	{
		FSpawnObjectData ObjData{ pool };

		GetPossiblePositionsOfObject(ObjData, pool.Object, Cell, CurrentZoneData, CurrentCells);

		if (ObjData.EnabledPositions.Cells.Num() > 0)
		{
			ArrayToFill.Add(ObjData);
		}
	}


}


void AOmGGrid::GetPossiblePositionsOfObject(FSpawnObjectData& StructToFill, EObjectType Object, TPair<FPositionInGrid, EZoneType> Cell,
	TMap<EZoneType, FZoneGameplayData>& CurrentZoneData, TMap<FVector2D, FSlotData>& CurrentCells)
{
	TArray<FArray2D> PossibleProvisions;
	FMapPositionZone SpawnPosData;


	switch (Object)
	{
	case EObjectType::Single:

		SpawnPosData.Map = TMap<FVector2D, EZoneType>{ TTuple<FVector2D, EZoneType>{ FVector2D{float(Cell.Key.Row), float(Cell.Key.Column)}, Cell.Value } };
		StructToFill.EnabledPositions.Cells.Add(SpawnPosData);

		return;

	case EObjectType::Double:

		PossibleProvisions = TArray<FArray2D>{
		FArray2D{ TArray<FPositionInGrid> {FPositionInGrid{0,-1}}},
		FArray2D{ TArray<FPositionInGrid> {FPositionInGrid{-1,0}}},
		FArray2D{ TArray<FPositionInGrid> {FPositionInGrid{0,1}} },
		FArray2D{ TArray<FPositionInGrid> {FPositionInGrid{1,0}} }
		};

		break;

	case EObjectType::Corner:

		PossibleProvisions = TArray<FArray2D>{
		FArray2D{ TArray<FPositionInGrid> {FPositionInGrid{0,-1}, FPositionInGrid{1,-1}}},
		FArray2D{ TArray<FPositionInGrid> {FPositionInGrid{-1,0}, FPositionInGrid{-1,-1}}},
		FArray2D{ TArray<FPositionInGrid> {FPositionInGrid{0,1}, FPositionInGrid{-1,1}}},
		FArray2D{ TArray<FPositionInGrid> {FPositionInGrid{0,1}, FPositionInGrid{1,1}}},
		FArray2D{ TArray<FPositionInGrid> {FPositionInGrid{1,0}, FPositionInGrid{1,1}}},
		FArray2D{ TArray<FPositionInGrid> {FPositionInGrid{0,-1}, FPositionInGrid{-1,-1}}},
		FArray2D{ TArray<FPositionInGrid> {FPositionInGrid{-1,0}, FPositionInGrid{-1,1}}},
		FArray2D{ TArray<FPositionInGrid> {FPositionInGrid{0,1}, FPositionInGrid{1,1}}},
		FArray2D{ TArray<FPositionInGrid> {FPositionInGrid{1,0}, FPositionInGrid{1,-1}}}
		};

		break;

	case EObjectType::Quadruple:

		PossibleProvisions = TArray<FArray2D>{
		FArray2D{ TArray<FPositionInGrid> {FPositionInGrid{0,1}, FPositionInGrid{0,2}, FPositionInGrid{-1,1}}},
		FArray2D{ TArray<FPositionInGrid> {FPositionInGrid{0,1}, FPositionInGrid{0,2}, FPositionInGrid{1,1}}},
		FArray2D{ TArray<FPositionInGrid> {FPositionInGrid{1,0}, FPositionInGrid{2,0}, FPositionInGrid{1,-1}}},
		FArray2D{ TArray<FPositionInGrid> {FPositionInGrid{1,0}, FPositionInGrid{2,0}, FPositionInGrid{1,1}}}
		};

		break;

	case EObjectType::Square:

		PossibleProvisions = TArray<FArray2D>{
		FArray2D{ TArray<FPositionInGrid> {FPositionInGrid{-1,0}, FPositionInGrid{-1,1}, FPositionInGrid{0,1}}},
		FArray2D{ TArray<FPositionInGrid> {FPositionInGrid{0,1}, FPositionInGrid{1,1}, FPositionInGrid{1,0}}},
		FArray2D{ TArray<FPositionInGrid> {FPositionInGrid{1,0}, FPositionInGrid{1,-1}, FPositionInGrid{0,-1}}},
		FArray2D{ TArray<FPositionInGrid> {FPositionInGrid{0,-1}, FPositionInGrid{-1,-1}, FPositionInGrid{-1,0}}}
		};

		break;

	default:
		return;
	}

	for (auto& prov : PossibleProvisions)
	{
		bool bCanBeAdded = true;

		for (auto& arrPos : prov.cells)
		{
			TMap<FVector2D, EZoneType> posToAdd;

			arrPos.Row += Cell.Key.Row;
			arrPos.Column += Cell.Key.Column;

			EZoneType zoneT = IsCellFree(arrPos, CurrentZoneData);

			if (zoneT != EZoneType::ZoneNone)
			{
				SpawnPosData.Map.Add(TTuple<FVector2D, EZoneType>{ FVector2D{ float(Cell.Key.Row), float(Cell.Key.Column) }, Cell.Value });
				SpawnPosData.Map.Add(TTuple<FVector2D, EZoneType>{ FVector2D{ float(arrPos.Row), float(arrPos.Column) }, zoneT });
			}
			else
			{
				bCanBeAdded = false;
				break;
			}
		}

		if (bCanBeAdded)
		{
			StructToFill.EnabledPositions.Cells.Add(SpawnPosData);
		}

		SpawnPosData.Map.Empty();
	}



}

EZoneType AOmGGrid::IsCellFree(FPositionInGrid CellPos, TMap<EZoneType, FZoneGameplayData>& CurrentZoneData) const
{
	for (auto& zoneDat : CurrentZoneData)
	{
		if (zoneDat.Value.Cells.Contains(CellPos))
		{
			return zoneDat.Key;
		}
	}



	return EZoneType::ZoneNone;
}





void AOmGGrid::ProvideObjectSpawning(FObjectData ObjectData, TMap<FVector2D, EZoneType> SpawnPos, EZoneType ObjZoneType,
	TMap<EZoneType, FZoneGameplayData>& CurrentZoneData, TMap<FVector2D, FSlotData>& CurrentCells)
{
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	SpawnParams.Owner = GetWorld()->GetFirstPlayerController();



	if (AOmGObjectBase* obj = GetWorld()->SpawnActor<AOmGObjectBase>(SpawningObjectClass, GetActorLocation(), FRotator(0.0f), SpawnParams))
	{
		obj->InitializeObject(SpawnPos, ObjectData.Object, ObjectData.HP, ObjZoneType);
		Objects.Add(obj);

		TArray<FPositionInGrid> AllNeighbors;

		for (auto& cell : SpawnPos)
		{
			BanCell(FPositionInGrid{ int32(cell.Key.X), int32(cell.Key.Y) }, CurrentZoneData, CurrentCells, obj);

			GetAllCellNeighbors(FPositionInGrid{ int32(cell.Key.X), int32(cell.Key.Y) }, AllNeighbors);

		}

		for (int32 i = 0; i < AllNeighbors.Num(); ++i)
		{
			BanCell(AllNeighbors[i], CurrentZoneData, CurrentCells);
		}
	}


	CurrentHealth += ObjectData.HP;	
}


void AOmGGrid::GetAllCellNeighbors(FPositionInGrid CellPos, TArray<FPositionInGrid>& ArrayToFill) const
{
	if (CellPos.Column - 1 >= 0)
	{
		ArrayToFill.AddUnique(FPositionInGrid{ CellPos.Row, CellPos.Column - 1 });
	}

	if (CellPos.Row - 1 >= 0 && CellPos.Column - 1 >= 0)
	{
		ArrayToFill.AddUnique(FPositionInGrid{ CellPos.Row - 1, CellPos.Column - 1 });
	}

	if (CellPos.Row - 1 >= 0)
	{
		ArrayToFill.AddUnique(FPositionInGrid{ CellPos.Row - 1, CellPos.Column });
	}

	if (CellPos.Row - 1 >= 0 && CellPos.Column + 1 <= Columns - 1)
	{
		ArrayToFill.AddUnique(FPositionInGrid{ CellPos.Row - 1, CellPos.Column + 1 });
	}

	if (CellPos.Column + 1 <= Columns - 1)
	{
		ArrayToFill.AddUnique(FPositionInGrid{ CellPos.Row, CellPos.Column + 1 });
	}

	if (CellPos.Row + 1 <= Rows - 1 && CellPos.Column + 1 <= Columns - 1)
	{
		ArrayToFill.AddUnique(FPositionInGrid{ CellPos.Row + 1, CellPos.Column + 1 });
	}

	if (CellPos.Row + 1 <= Rows - 1)
	{
		ArrayToFill.AddUnique(FPositionInGrid{ CellPos.Row + 1, CellPos.Column });
	}


	if (CellPos.Row + 1 <= Rows - 1 && CellPos.Column - 1 >= 0)
	{
		ArrayToFill.AddUnique(FPositionInGrid{ CellPos.Row + 1, CellPos.Column - 1 });
	}
}