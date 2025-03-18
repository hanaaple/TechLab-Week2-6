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
	float Spacing = 0.2f;
	TArray<ULineComp*> gridLines;
	float maxGridDistance = 50;
};
