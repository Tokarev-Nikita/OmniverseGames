

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "OmGCoreTypes.h"

#include "OmGSubobject.generated.h"



UCLASS()
class OMNIVERSEGAMES_API AOmGSubobject : public AActor
{
	GENERATED_BODY()
	
public:	
	
	AOmGSubobject();

	void InitializeSubobject(AOmGObjectBase* ObjectClass, FPositionInGrid Position, EZoneType Zone, EZoneType ObjectZone, int32 HP);

protected:
	UPROPERTY(VisibleAnywhere)
	USceneComponent* SceneComponent;

	UPROPERTY(BlueprintReadOnly)
	AOmGObjectBase* ParentObject = nullptr;

	UPROPERTY(BlueprintReadOnly)
	FPositionInGrid PositionInGrid;

	UPROPERTY(BlueprintReadOnly)
	EZoneType ZoneType = EZoneType::ZoneNone;

	UPROPERTY(BlueprintReadOnly)
	EZoneType ObjectZoneType = EZoneType::ZoneNone;

	UPROPERTY(BlueprintReadOnly)
	int32 BlockHealth = 3;

protected:
	
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintImplementableEvent, Category = "Blueprints")
	void ConfigSubobjectDisplay();

};
