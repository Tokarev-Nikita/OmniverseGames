#pragma once

#include "OmGCoreTypes.generated.h"

class AOmGObjectBase;
class AOmGSubobject;

UENUM(BlueprintType)
enum class EZoneType : uint8
{
	ZoneNone UMETA(DisplayName, "Zone None"),
	ZoneI UMETA(DisplayName, "Zone I"),
	ZoneII UMETA(DisplayName, "Zone II"),
	ZoneIII UMETA(DisplayName, "Zone III")
};

UENUM(BlueprintType)
enum class EObjectType : uint8
{
	Single UMETA(DisplayName, "Single"),
	Double UMETA(DisplayName, "Double"),
	Corner UMETA(DisplayName, "Corner"),
	Quadruple UMETA(DisplayName, "Quadruple"),
	Square UMETA(DisplayName, "Square")
};



USTRUCT(BlueprintType)
struct FPositionInGrid
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ClampMin = "0"))
	int32 Row;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ClampMin = "0"))
	int32 Column;	

	bool operator==(const FPositionInGrid& Pos) const
	{
		return Pos.Row == this->Row && Pos.Column == this->Column;
	}	
};



USTRUCT(BlueprintType)
struct FZoneData
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EZoneType Zone = EZoneType::ZoneI;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ClampMin = "-1"))
	int32 Size = -1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FPositionInGrid StartPosition;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FPositionInGrid EndPosition;
};


USTRUCT(BlueprintType)
struct FObjectData
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EObjectType Object = EObjectType::Single;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ClampMin = "1", ClampMax = "100"))
	int32 Weight = 5;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ClampMin = "-1"))
	int32 Limit = 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ClampMin = "3"))
	int32 HP = 3;

	UPROPERTY(BlueprintReadOnly)
	int32 BaseLimit = Limit;
};

USTRUCT(BlueprintType)
struct FObjectPool
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FObjectData> ObjectsData;

	void IncreasePool(float MultValue)
	{
		for (auto& object : ObjectsData)
		{
			if (object.Limit != -1)
			{
				object.Limit += object.BaseLimit * MultValue;
			}
		}
	}

};


USTRUCT(BlueprintType)
struct FZoneGameplayData
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<FPositionInGrid> Cells;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FObjectPool Objects;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 Priority;
};


USTRUCT(BlueprintType)
struct FSlotData
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	EZoneType Zone = EZoneType::ZoneNone;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	bool bIsBusy = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	AOmGObjectBase* ObjectRef = nullptr;
};


USTRUCT(BlueprintType)
struct FZoneDataPair
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FZoneGameplayData ZoneDat;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	EZoneType Zone = EZoneType::ZoneNone;
};


USTRUCT(BlueprintType)
struct FMapPositionZone
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TMap<FVector2D, EZoneType> Map;

};

USTRUCT(BlueprintType)
struct FEnabledObjectSpawnPositions
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<FMapPositionZone> Cells;

	/*UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TMap<FVector2D, EZoneType> Cells;*/
};


USTRUCT(BlueprintType)
struct FSpawnObjectData
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FObjectData ObjectData;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FEnabledObjectSpawnPositions EnabledPositions;
};



USTRUCT(BlueprintType)
struct FObjectCellData
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FPositionInGrid CellPos;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	EZoneType Zone = EZoneType::ZoneNone;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	AOmGSubobject* SubobjectRef;
};



USTRUCT(BlueprintType)
struct FObjectGameplayData
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	EObjectType ObjectType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 BlockHealth;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	EZoneType ZoneType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<FObjectCellData> SubobjectsData;
};



USTRUCT(BlueprintType)
struct FArray2D
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<FPositionInGrid> cells;;
};