#include "Object/Gizmo/WorldGrid.h"

AWorldGrid::AWorldGrid()
{
	bIsGizmo = true;

	ULineComp* line = AddComponent<ULineComp>();
	//gridLines[0]->SetRenderMode(ERenderMode::Batch);
	FTransform transform = line->GetRelativeTransform();
	transform.SetScale(FVector(maxGridDistance, 1, 1));
	line->SetRelativeTransform(transform);
	line->SetCustomColor(FVector4(1.0f, 1.0f, 1.0f, 1.0f));
	RootComponent = line;
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
	int q3 = q1 * 3;
	int q4 = q1 * 4;

	int num = gridLines.Num();

	for (int i = 0; i < num; i++) {
		RemoveComponent(gridLines[i]);
		gridLines[i] = nullptr;
	}
	gridLines.Empty();

	for (int i = 0; i < q1; i++) {
		ULineComp* line = AddComponent<ULineComp>();
		gridLines.Add(line);
		//gridLines[i]->SetRenderMode(ERenderMode::Batch);
		FTransform transform = gridLines[i]->GetRelativeTransform();
		transform.SetScale(FVector(maxGridDistance, 1, 1));
		transform.SetPosition(FVector(0, (i + 1) * Spacing, 0));
		gridLines[i]->SetRelativeTransform(transform);
		gridLines[i]->SetCustomColor(FVector4(1.0f, 1.0f, 1.0f, 1.0f));
		gridLines[i]->SetupAttachment(RootComponent);
	}
	for (int i = q1; i < q2; i++) {
		ULineComp* line = AddComponent<ULineComp>();
		gridLines.Add(line);
		//gridLines[i]->SetRenderMode(ERenderMode::Batch);
		FTransform transform = gridLines[i]->GetRelativeTransform();
		transform.SetScale(FVector(maxGridDistance, 1, 1));
		transform.SetPosition(FVector(0, -(i - (q1 - 1)) * Spacing, 0));
		gridLines[i]->SetRelativeTransform(transform);
		gridLines[i]->SetCustomColor(FVector4(1.0f, 1.0f, 1.0f, 1.0f));
		gridLines[i]->SetupAttachment(RootComponent);
	}

	for (int i = q2; i < q3; i++) {
		ULineComp* line = AddComponent<ULineComp>();
		gridLines.Add(line);
		//gridLines[i]->SetRenderMode(ERenderMode::Batch);
		FTransform transform = gridLines[i]->GetRelativeTransform();
		transform.SetScale(FVector(maxGridDistance, 1, 1));
		transform.SetPosition(FVector((i - (q2 - 1)) * Spacing, 0, 0));
		transform.SetRotation(FVector(0, 0, 90));
		gridLines[i]->SetRelativeTransform(transform);
		gridLines[i]->SetCustomColor(FVector4(1.0f, 1.0f, 1.0f, 1.0f));
		gridLines[i]->SetupAttachment(RootComponent);
	}
	for (int i = q3; i < q4; i++) {
		ULineComp* line = AddComponent<ULineComp>();
		gridLines.Add(line);
		//gridLines[i]->SetRenderMode(ERenderMode::Batch);
		FTransform transform = gridLines[i]->GetRelativeTransform();
		transform.SetScale(FVector(maxGridDistance, 1, 1));
		transform.SetPosition(FVector(-(i - (q3 - 1)) * Spacing, 0, 0));
		transform.SetRotation(FVector(0, 0, 90));
		gridLines[i]->SetRelativeTransform(transform);
		gridLines[i]->SetCustomColor(FVector4(1.0f, 1.0f, 1.0f, 1.0f));
		gridLines[i]->SetupAttachment(RootComponent);
	}
}
