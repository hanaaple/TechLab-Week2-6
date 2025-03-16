#pragma once
#include "Object/Actor/Actor.h"
#include "Object/PrimitiveComponent/UPrimitiveComponent.h"

class ABoundingBoxActor : public AActor 
{
	DECLARE_OBJECT(ABoundingBoxActor, AActor)
public:
	FVector Min;
	FVector Max;
public: 
	ABoundingBoxActor();

public:
	virtual void Tick(float DeltaTime) override;
	void SetActorVisibility(bool bNewActive);
	void UpdateTransform();
	virtual const char* GetTypeName() override;
private:
	bool bIsActive = false;
};