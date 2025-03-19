#include "Object/Gizmo/WorldGrid.h"
#include "Core/SettingManager.h"

AWorldGrid::AWorldGrid()
{
	bIsGizmo = true;
	Spacing = SettingManager::Get().LoadGridSpacing();
	RootComponent = AddComponent<USceneComponent>();
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
	UpdateGrid();
}

void AWorldGrid::UpdateGrid()
{
	int LineCount = (maxGridDistance * 2) / Spacing;

	int PrevLineCount = HorizontalGridLines.Num();

	if (LineCount > PrevLineCount)
	{
		for (int i = 0; i < LineCount-PrevLineCount; i++)
		{
			CreateLine(true);
			CreateLine(false);
		}
	}
	else if (LineCount < PrevLineCount)
	{
		for (int i = 0; i < PrevLineCount-LineCount; i++)
		{
			RemoveLine(true);
			RemoveLine(false);
		}
	}

	float Offset = 0;
	for (int i = 0; i < LineCount / 2; i++)
	{
		ULineComp* Line = VerticalGridLines[i];
		FTransform Transform = Line->GetRelativeTransform();
		Transform.SetPosition(0, Offset, 0);
		Line->SetRelativeTransform(Transform);
		Offset += Spacing;
	}
	Offset = -Spacing;
	for (int i = LineCount / 2; i < LineCount; i++)
	{
		ULineComp* Line = VerticalGridLines[i];
		FTransform Transform = Line->GetRelativeTransform();
		Transform.SetPosition(0, Offset, 0);
		Line->SetRelativeTransform(Transform);
		Offset -= Spacing;
	} 

	
	Offset = 0;
	for (int i = 0; i < LineCount / 2; i++)
	{
		ULineComp* Line = HorizontalGridLines[i];
		FTransform Transform = Line->GetRelativeTransform();
		Transform.SetPosition(Offset, 0, 0);
		Line->SetRelativeTransform(Transform);
		Offset += Spacing;
	}
	Offset = -Spacing;
	for (int i = LineCount / 2; i < LineCount; i++)
	{
		ULineComp* Line = HorizontalGridLines[i];
		FTransform Transform = Line->GetRelativeTransform();
		Transform.SetPosition(Offset, 0, 0);
		Line->SetRelativeTransform(Transform);
		Offset -= Spacing;
	} 
}
void AWorldGrid::CreateLine(bool isVertical)
{
	ULineComp* Line = AddComponent<ULineComp>();
	
	Line->SetRenderMode(ERenderMode::Batch);
	Line->SetCustomColor(FVector4(1.0f, 1.0f, 1.0f, 1.0f));
	Line->SetupAttachment(RootComponent);

	FTransform Transform = Line->GetRelativeTransform();
	Transform.SetScale(FVector(maxGridDistance * 2, 1, 1));

	if (isVertical)
	{
		VerticalGridLines.Add(Line);
		Transform.SetRotation(FVector(0, 0, 0));
	}
	else
	{
		HorizontalGridLines.Add(Line);
		Transform.SetRotation(FVector(0, 0, 90));
	}
	
	Line->SetRelativeTransform(Transform);
}

void AWorldGrid::RemoveLine(bool isVertical)
{
	if (isVertical)
	{
		auto* Line = VerticalGridLines[0];
		VerticalGridLines.RemoveAt(0);
		RemoveComponent(Line);
	}
	else
	{
		auto* Line = HorizontalGridLines[0];
		HorizontalGridLines.RemoveAt(0);
		RemoveComponent(Line);
	}
}