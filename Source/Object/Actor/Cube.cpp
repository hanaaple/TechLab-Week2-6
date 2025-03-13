#include "Cube.h"
#include <Object/PrimitiveComponent/UPrimitiveComponent.h>

ACube::ACube()
{
	bCanEverTick = true;

	UCubeComp* CubeComponent = AddComponent<UCubeComp>();
	RootComponent = CubeComponent;

	CubeComponent->SetRelativeTransform(FTransform());
}

void ACube::BeginPlay()
{
	Super::BeginPlay();
}

void ACube::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
    std::cout<<GetClassFName().ToString().ToStdString()<<std::endl;
}

const char* ACube::GetTypeName()
{
	return "Cube";
}
