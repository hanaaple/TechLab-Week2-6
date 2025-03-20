#pragma once
#include "Object/Actor/Actor.h"
#include <Object/PrimitiveComponent/UPrimitiveComponent.h>

class AWorldGrid : public AActor
{
	DECLARE_OBJECT(AWorldGrid, AActor)
	using Super = AActor;
public:
	AWorldGrid();
	virtual ~AWorldGrid() = default;

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual const char* GetTypeName() override;
	void SetSpacing(float value);
	float GetSpacing() { return Spacing; }
	void UpdateGrid();
private:
	void CreateLine(bool isVertical);
	void RemoveLine(bool isVertical);
	
private:
	float Spacing;
	TArray<ULineComp*> HorizontalGridLines;
	TArray<ULineComp*> VerticalGridLines;
	float maxGridDistance = 100;
};
