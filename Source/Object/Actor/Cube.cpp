#include "Cube.h"
#include <Object/PrimitiveComponent/UPrimitiveComponent.h>

#include "Arrow.h"

ACube::ACube()
{
	bCanEverTick = true;

	UCubeComp* CubeComponent = AddComponent<UCubeComp>();
	RootComponent = CubeComponent;

	CubeComponent->SetRelativeTransform(FTransform());
	std::cout<<StaticClassFName_Internal().ToString().ToStdString()<<" "<<std::endl;
	std::cout << "ACube IsA AActor? " << IsA<AActor>()<<" "<<IsAByName("AActor") << std::endl;
	std::cout << "ACube IsA UObject? " << IsA<UObject>() <<" "<<IsAByName("UObject") << std::endl;
	std::cout << "ACube IsA ACube? " << IsA<ACube>() <<" "<<IsAByName("ACube") << std::endl;
	std::cout << "ACube IsA OtherClass? " << IsA<AArrow>()<<" "<<IsAByName("AArrow")  << std::endl;
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
