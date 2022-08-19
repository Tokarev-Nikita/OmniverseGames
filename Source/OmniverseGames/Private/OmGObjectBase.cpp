


#include "OmGObjectBase.h"
#include "OmGSubobject.h"


AOmGObjectBase::AOmGObjectBase()
{
 	
	PrimaryActorTick.bCanEverTick = false;

	SceneComponent = CreateDefaultSubobject<USceneComponent>("RootComponent");
	SetRootComponent(SceneComponent);

}



void AOmGObjectBase::BeginPlay()
{
	Super::BeginPlay();
	
}


void AOmGObjectBase::InitializeObject(TMap<FVector2D, EZoneType> ObjCells, EObjectType ObjectType, int32 BlockHelath, EZoneType ObjZoneType)
{	
	ObjectData.ObjectType = ObjectType;
	ObjectData.BlockHealth = BlockHelath;
	ObjectData.ZoneType = ObjZoneType;

	int32 SubobjectBlockHP = BlockHelath / ObjCells.Num();

	for (auto& objCell : ObjCells)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		SpawnParams.Owner = GetWorld()->GetFirstPlayerController();

		if (AOmGSubobject* Subobj = GetWorld()->SpawnActor<AOmGSubobject>(SubobjectClass, GetActorLocation(), FRotator(0.0f), SpawnParams))
		{
			FPositionInGrid pos{int32(objCell.Key.X), int32(objCell.Key.Y)};

			Subobj->InitializeSubobject(this, pos, objCell.Value, ObjectData.ZoneType, SubobjectBlockHP);

			const FObjectCellData dat{ pos, objCell.Value, Subobj };

			ObjectData.SubobjectsData.Add(dat);
		}
		
	}
}



void AOmGObjectBase::DestroyObj()
{	

	for (auto& sub : ObjectData.SubobjectsData)
	{
		if (sub.SubobjectRef)
		{
			sub.SubobjectRef->Destroy();
		}		
	}

	this->Destroy();
}

