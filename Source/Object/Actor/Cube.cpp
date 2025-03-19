#include "Cube.h"
#include <Object/PrimitiveComponent/UPrimitiveComponent.h>

ACube::ACube()
{
	bCanEverTick = true;

	UCubeComp* CubeComponent = AddComponent<UCubeComp>();
	RootComponent = CubeComponent;
	CubeComponent->SetRelativeTransform(FTransform());
	/*
	std::cout << IsA<AActor>() << std::endl;   // (1) Type 기반 확인
	std::cout << IsA("AActor") << std::endl;   // (2) Name 기반 확인
	std::cout << IsA<UObject>() << std::endl;  // (3) UObject 상속 여부 확인
	std::cout << IsA("UObject") << std::endl;  // (4) UObject 이름 확인
	std::cout << IsA<ACube>() << std::endl;    // (5) 자기 자신 확인
	std::cout << IsA("ACube") << std::endl;    // (6) 자기 자신 이름 확인
	std::cout << IsA<AArrow>() << std::endl;    
	std::cout << IsA("AArrow") << std::endl;   */
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
