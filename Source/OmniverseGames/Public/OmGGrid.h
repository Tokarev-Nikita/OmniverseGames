// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "OmGCoreTypes.h"

#include "OmGGrid.generated.h"

UCLASS()
class OMNIVERSEGAMES_API AOmGGrid : public AActor
{
	GENERATED_BODY()
	
public:	
	
	AOmGGrid();

protected:
	
	UPROPERTY(VisibleAnywhere)
	USceneComponent* SceneComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Main", meta = (ClampMin = "1", ClampMax = "20"))
	int32 Rows = 6;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Main", meta = (ClampMin = "1", ClampMax = "20"))
	int32 Columns = 6;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Main")
	TSubclassOf<AOmGObjectBase> SpawningObjectClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Main")
	bool bCellsHiddenInGame = false;

	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Spawn")
	TMap<EZoneType, int32> ZonePriorities;			

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Spawn")
	TMap<EZoneType, float> ObjectsSpawnChance;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Spawn")
	TMap<EZoneType, FObjectPool> ObjectPools;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Spawn")
	TArray<FZoneData> ZonesSpawnData;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Spawn", meta = (ClampMin = "3", ClampMax = "300"))
	int32 MaxFieldStrength = 30;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Spawn", meta = (ClampMin = "0"))
	int32 Lumbagos = 1;


	UPROPERTY(BlueprintReadWrite)
	TMap<EZoneType, FZoneGameplayData> ZoneData;

	UPROPERTY(BlueprintReadWrite)
	TMap<FVector2D, FSlotData> Cells;

	UPROPERTY(BlueprintReadOnly)
	int32 CurrentHealth = 0;

	UPROPERTY(BlueprintReadOnly)
	TArray<AOmGObjectBase*> Objects;


protected:

	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, Category = "Blueprints")
	void InitializeGrid(TMap<EZoneType, FZoneGameplayData>& zoness, TMap<FVector2D, FSlotData>& cellss);

	UFUNCTION(BlueprintCallable, Category = "Blueprints")
	void RegenerateGrid();

	UFUNCTION(BlueprintImplementableEvent, Category = "Blueprints")
	void LoadData();	

	UFUNCTION(BlueprintImplementableEvent, Category = "Blueprints")
	void VisualizeCell(FPositionInGrid Position, EZoneType Zone);

	UFUNCTION(BlueprintImplementableEvent, Category = "Blueprints")
	void VisualizeCellBanning(FPositionInGrid Position);

	UFUNCTION(BlueprintImplementableEvent, Category = "Blueprints")
	void GenerationFinished();



private:

	void CheckMainParams();
	void GenerateZoneData();
	void InitializeGridCells();
	bool IsCellInZone(FPositionInGrid CellPos, FPositionInGrid StartPos, FPositionInGrid EndPos, int32 ZoneSize) const;
	void AddCellInZone(FPositionInGrid CellPos, EZoneType Zone);
	void SortZoneDataByPriority();
	int32 GetPriorityIndex(TArray<FZoneDataPair>& data) const;
	void VisualizeGrid();
	

	void SpawnObjectsInGrid();
	void SetupRightHealthes();
	void GenerateLumbagos(TMap<EZoneType, FZoneGameplayData>& CurrentZoneData, TMap<FVector2D, FSlotData>& CurrentCells);
	void BanCell(FPositionInGrid CellPos, TMap<EZoneType, FZoneGameplayData>& CurrentZoneData, TMap<FVector2D, FSlotData>& CurrentCells, AOmGObjectBase* ObjRef = nullptr);
	void GetAllCellNeighbors(FPositionInGrid CellPos, TArray<FPositionInGrid>& ArrayToFill) const;

	void SpawnObject_Execution(TMap<EZoneType, FZoneGameplayData>& CurrentZoneData, TMap<FVector2D, FSlotData>& CurrentCells);
	TPair<FPositionInGrid, EZoneType> GetPointForSpawn(TMap<EZoneType, FZoneGameplayData> CurrentZoneData) const;
	void AddObjectInGrid(TPair<FPositionInGrid, EZoneType>& Cell, TMap<EZoneType, FZoneGameplayData>& CurrentZoneData, TMap<FVector2D, FSlotData>& CurrentCells);
	void GetAllEnabledObjectsForCell(TArray<FSpawnObjectData>& ArrayToFill, TPair<FPositionInGrid, EZoneType>& Cell,
		TMap<EZoneType, FZoneGameplayData>& CurrentZoneData, TMap<FVector2D, FSlotData>& CurrentCells);
	void GetPossiblePositionsOfObject(FSpawnObjectData& StructToFill, EObjectType Object, TPair<FPositionInGrid, EZoneType> Cell,
		TMap<EZoneType, FZoneGameplayData>& CurrentZoneData, TMap<FVector2D, FSlotData>& CurrentCells);
	EZoneType IsCellFree(FPositionInGrid CellPos, TMap<EZoneType, FZoneGameplayData>& CurrentZoneData) const;
	void ProvideObjectSpawning(FObjectData ObjectData, TMap<FVector2D, EZoneType> SpawnPos, EZoneType ObjZoneType,
		TMap<EZoneType, FZoneGameplayData>& CurrentZoneData, TMap<FVector2D, FSlotData>& CurrentCells);

};
