


#include "OmGSubobject.h"
#include "OmgObjectBase.h"

AOmGSubobject::AOmGSubobject()
{
 	
	PrimaryActorTick.bCanEverTick = false;

	SceneComponent = CreateDefaultSubobject<USceneComponent>("RootComponent");
	SetRootComponent(SceneComponent);

}



void AOmGSubobject::BeginPlay()
{
	Super::BeginPlay();
	
}


void AOmGSubobject::InitializeSubobject(AOmGObjectBase* ObjectClass, FPositionInGrid Position, EZoneType Zone, EZoneType ObjectZone, int32 HP)
{
	ParentObject = ObjectClass;
	PositionInGrid = Position;
	ZoneType = Zone;
	ObjectZoneType = ObjectZone;
	BlockHealth = HP;


	ConfigSubobjectDisplay();
}



