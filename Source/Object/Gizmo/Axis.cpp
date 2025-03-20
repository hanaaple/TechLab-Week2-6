#include "Axis.h"
#include "Object/PrimitiveComponent/UPrimitiveComponent.h"

AAxis::AAxis()
{
	bIsGizmo = true;

	ULineComp* LineX = AddComponent<ULineComp>();
	FTransform XTransform = LineX->GetRelativeTransform();
	XTransform.SetScale(FVector(1000.0f, 1.0f, 1.0f));
	XTransform.Rotate({0.0f, 0.0f, 0.0f});
	LineX->SetRelativeTransform(XTransform);
	LineX->SetCustomColor(FVector4(1.0f, 0.0f, 0.0f, 1.0f));
	
	RootComponent = LineX;

	ULineComp* LineY = AddComponent<ULineComp>();
	FTransform YTransform = LineY->GetRelativeTransform();
	YTransform.SetScale(FVector(1000.0f, 1.0f, 1.0f));
	YTransform.Rotate({0.0f, 0.0f, 90.0f});
	LineY->SetRelativeTransform(YTransform);
	LineY->SetCustomColor(FVector4(0.0f, 1.0f, 0.0f, 1.0f));
	

	ULineComp* LineZ = AddComponent<ULineComp>();
	FTransform ZTransform = LineZ->GetRelativeTransform();
	ZTransform.SetScale(FVector(1000.0f, 1.0f, 1.0f));
	ZTransform.Rotate({0.0f, 90.0f, 0.0f});
	LineZ->SetRelativeTransform(ZTransform);
	LineZ->SetCustomColor(FVector4(0.0f, 0.0f, 1.0f, 1.0f));

	LineY->SetupAttachment(RootComponent);
	LineZ->SetupAttachment(RootComponent);
	
	LineX->SetDepth(1000);
	LineY->SetDepth(1000);
	LineZ->SetDepth(1000);
} 

void AAxis::BeginPlay()
{
	Super::BeginPlay();
	
}

void AAxis::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

const char* AAxis::GetTypeName()
{
	return "Axis";
}
