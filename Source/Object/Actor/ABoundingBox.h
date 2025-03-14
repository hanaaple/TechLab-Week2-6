#pragma once
#include "Object/Actor/Actor.h"
#include "Object/PrimitiveComponent/UPrimitiveComponent.h"

class ABoundingBoxActor : public AActor 
{
public:
	FVector Min;
	FVector Max;
public: 
	ABoundingBoxActor();

public:
	virtual void Tick(float DeltaTime) override;
	void SetActive(bool bActive);
	void UpdateTransform();

private:
	bool bIsActive = false;
	UBoundingBoxComp* boundingBoxComponent;
};