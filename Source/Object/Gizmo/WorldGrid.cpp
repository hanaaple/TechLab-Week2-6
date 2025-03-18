#include "Object/Gizmo/WorldGrid.h"

AWorldGrid::AWorldGrid()
{
	bIsGizmo = true;

	ULineComp* line = AddComponent<ULineComp>();
	//line->SetRenderMode(ERenderMode::Batch);
	FTransform transform = line->GetRelativeTransform();
	transform.SetScale(FVector(maxGridDistance, 1, 1));
	line->SetRelativeTransform(transform);
	line->SetCustomColor(FVector4(1.0f, 1.0f, 1.0f, 1.0f));
	RootComponent = line;
	for (int i = 0; i < (maxGridDistance / Spacing) * 2; i++) {
		ULineComp* verticalLine = AddComponent<ULineComp>();
		ULineComp* horizontalLine = AddComponent<ULineComp>();
		VerticalGridLines.Add(verticalLine);
		HorizontalGridLines.Add(horizontalLine);
		//verticalLine->SetRenderMode(ERenderMode::Batch);
		//horizontalLine->SetRenderMode(ERenderMode::Batch);
		FTransform vtransform = verticalLine->GetRelativeTransform();
		vtransform.SetScale(FVector(maxGridDistance, 1, 1));
		verticalLine->SetRelativeTransform(vtransform);
		verticalLine->SetCustomColor(FVector4(1.0f, 1.0f, 1.0f, 1.0f));
		VerticalGridLines[i]->SetupAttachment(RootComponent);
		FTransform htransform = horizontalLine->GetRelativeTransform();
		htransform.SetScale(FVector(maxGridDistance, 1, 1));
		htransform.SetRotation(FVector(0, 0, 90));
		horizontalLine->SetRelativeTransform(htransform);
		horizontalLine->SetCustomColor(FVector4(1.0f, 1.0f, 1.0f, 1.0f));
		HorizontalGridLines[i]->SetupAttachment(RootComponent);
	}
	UpdateGrid();
}

void AWorldGrid::BeginPlay()
{
	Super::BeginPlay();
}

void AWorldGrid::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

const char* AWorldGrid::GetTypeName()
{
	return "WorldGrid";
}

void AWorldGrid::SetSpacing(float value)
{
	Spacing = value;
}

void AWorldGrid::UpdateGrid()
{
	int q1 = maxGridDistance / Spacing;
	int q2 = q1 * 2;

	int num = HorizontalGridLines.Num();

	if (q2 < num) {
		for (int i = num - 1; i >= q2; i--) {
			RemoveComponent(HorizontalGridLines[i]);
			RemoveComponent(VerticalGridLines[i]);
			HorizontalGridLines.RemoveAt(i);
			VerticalGridLines.RemoveAt(i);
		}
	}
	else {
		for (int i = num; i < q2; i++) {
			ULineComp* VerticalLine = AddComponent<ULineComp>();
			VerticalGridLines.Add(VerticalLine);
			//VerticalGridLines[i]->SetRenderMode(ERenderMode::Batch);
			FTransform vtransform = VerticalGridLines[i]->GetRelativeTransform();
			vtransform.SetScale(FVector(maxGridDistance, 1, 1));
			VerticalGridLines[i]->SetRelativeTransform(vtransform);
			VerticalGridLines[i]->SetCustomColor(FVector4(1.0f, 1.0f, 1.0f, 1.0f));
			VerticalGridLines[i]->SetupAttachment(RootComponent);
			ULineComp* HorizontalLine = AddComponent<ULineComp>();
			HorizontalGridLines.Add(HorizontalLine);
			//HorizontalGridLines[i]->SetRenderMode(ERenderMode::Batch);
			FTransform htransform = HorizontalGridLines[i]->GetRelativeTransform();
			htransform.SetScale(FVector(maxGridDistance, 1, 1));
			htransform.SetRotation(FVector(0, 0, 90));   
			HorizontalGridLines[i]->SetRelativeTransform(htransform);
			HorizontalGridLines[i]->SetCustomColor(FVector4(1.0f, 1.0f, 1.0f, 1.0f));
			HorizontalGridLines[i]->SetupAttachment(RootComponent);
		}
	}

	for (int i = 0; i < q1; i++) {
		FTransform vTransform = VerticalGridLines[i]->GetComponentTransform();
		vTransform.SetPosition(0, (i + 1) * Spacing, 0);
		vTransform.SetScale(FVector(maxGridDistance, 1, 1));
		VerticalGridLines[i]->SetWorldTransform(vTransform);
		FTransform hTransform = HorizontalGridLines[i]->GetComponentTransform();
		hTransform.SetPosition((i + 1) * Spacing, 0, 0);
		hTransform.SetScale(FVector(maxGridDistance, 1, 1));
		HorizontalGridLines[i]->SetWorldTransform(hTransform);
	}
	for (int i = q1; i < q2; i++) {
		FTransform vTransform = VerticalGridLines[i]->GetComponentTransform();
		vTransform.SetPosition(0, -(i - (q1 - 1)) * Spacing, 0);
		vTransform.SetScale(FVector(maxGridDistance, 1, 1));
		VerticalGridLines[i]->SetWorldTransform(vTransform);
		FTransform hTransform = HorizontalGridLines[i]->GetComponentTransform();
		hTransform.SetPosition(-(i - (q1 - 1)) * Spacing, 0, 0);
		hTransform.SetScale(FVector(maxGridDistance, 1, 1));
		HorizontalGridLines[i]->SetWorldTransform(hTransform);
	}
}
