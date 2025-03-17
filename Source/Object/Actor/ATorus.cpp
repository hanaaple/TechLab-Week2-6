#include "ATorus.h"

#include <Object/PrimitiveComponent/UPrimitiveComponent.h>

ATorus::ATorus()
{
    bCanEverTick = true;

    UTorusComp* ConeComponent = AddComponent<UTorusComp>();
    RootComponent = ConeComponent;
	
    SetActorRelativeTransform(FTransform());
}

void ATorus::BeginPlay()
{
    Super::BeginPlay();
}

void ATorus::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

const char* ATorus::GetTypeName()
{
    return "Torus";
}
