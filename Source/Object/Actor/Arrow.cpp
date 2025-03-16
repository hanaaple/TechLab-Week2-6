#include "Arrow.h"
#include "Object/PrimitiveComponent/UPrimitiveComponent.h"

AArrow::AArrow()
{
	bCanEverTick = true;

	UCylinderComp* CylinderComp = AddComponent<UCylinderComp>();
	RootComponent = CylinderComp;

	RootComponent->SetRelativeTransform(FTransform(FVector(0.0f, 0.0f, 0.5f), FVector(), FVector(1.f, 1.f, 1.f)));

	UConeComp* ConeComp = AddComponent<UConeComp>();
	ConeComp->SetRelativeTransform(FTransform(FVector(0.0f, 0.0f, 1.0f), FVector(), FVector(1.2f, 1.2f, 0.5f)));

	ConeComp->SetupAttachment(RootComponent, EEndPlayReason::EAttachmentRule::KeepWorld);
	
	for (int i=0;i<10;i++)
	{
		UConeComp* ConeComp1 = AddComponent<UConeComp>();
		//ConeComp1->SetRelativeTransform(FTransform(FVector(0.0f, 0.0f, 1.0f), FVector(), FVector(1.2f, 1.2f, 0.5f)));
		ConeComp1->SetupAttachment(RootComponent, EEndPlayReason::EAttachmentRule::KeepWorld);

		for (int i=0;i<10;i++)
		{
			UConeComp* ConeComp2 = AddComponent<UConeComp>();
			//ConeComp1->SetRelativeTransform(FTransform(FVector(0.0f, 0.0f, 1.0f), FVector(), FVector(1.2f, 1.2f, 0.5f)));
			ConeComp2->SetupAttachment(ConeComp1, EEndPlayReason::EAttachmentRule::KeepWorld);
		}
	}
}

void AArrow::BeginPlay()
{
	Super::BeginPlay();
}

void AArrow::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

const char* AArrow::GetTypeName()
{
	return "Arrow";
}
