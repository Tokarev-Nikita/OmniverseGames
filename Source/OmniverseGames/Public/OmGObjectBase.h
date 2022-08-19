
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "OmGCoreTypes.h"

#include "OmGObjectBase.generated.h"

class AOmGSubobject;

UCLASS()
class OMNIVERSEGAMES_API AOmGObjectBase : public AActor
{
	GENERATED_BODY()
	
public:	
	
	AOmGObjectBase();

	void InitializeObject(TMap<FVector2D, EZoneType> ObjCells, EObjectType ObjectType, int32 BlockHelath, EZoneType ObjZoneType);

	void DestroyObj();

protected:
	UPROPERTY(VisibleAnywhere)
	USceneComponent* SceneComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<AOmGSubobject> SubobjectClass;

	UPROPERTY(BlueprintReadOnly)
	FObjectGameplayData ObjectData;
	

protected:
	
	virtual void BeginPlay() override;


};
