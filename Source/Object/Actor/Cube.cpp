#include "Cube.h"
#include <Object/PrimitiveComponent/UPrimitiveComponent.h>

ACube::ACube()
{
	bCanEverTick = true;

	UCubeComp* CubeComponent = AddComponent<UCubeComp>();
	RootComponent = CubeComponent;

	CubeComponent->SetRelativeTransform(FTransform());
	std::cout << "ACube IsA AActor? " << IsA("AActor") << std::endl;
	std::cout << "ACube IsA UObject? " << IsA("UObject") << std::endl;
	std::cout << "ACube IsA ACube? " << IsA("ACube") << std::endl;
	std::cout << "ACube IsA OtherClass? " << IsA("OtherClass") << std::endl;
	//std::cout<<Super::GetClassFName().ToString().ToStdString()<<std::endl;
	
	//std::cout<<IsA(FName("UObject"))<<" "<<Super::IsA(FName("UObject"))<<std::endl;
}

void ACube::BeginPlay()
{
	Super::BeginPlay();
}

void ACube::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

const char* ACube::GetTypeName()
{
	return "Cube";
}
