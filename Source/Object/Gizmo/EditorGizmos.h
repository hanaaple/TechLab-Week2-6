#pragma once
#include "Object/Actor/Actor.h"
#include "Object/PrimitiveComponent/UPrimitiveComponent.h"
#include "Core/Math/Vector4.h"

enum class ESelectedAxis : uint8
{
	None,
	X,
	Y,
	Z
};

class AEditorGizmos : public AActor
{
    DECLARE_OBJECT(AEditorGizmos, AActor)
public:
	AEditorGizmos();

public:
	virtual void Tick(float DeltaTime) override;
	void SetScaleByDistance();
	void SetActorVisibility(bool bNewActive);
	void SetSelectedAxis(ESelectedAxis NewAxis) { SelectedAxis = NewAxis; }
	TArray<UPrimitiveComponent*> GetAxis() { return axisComponents; }
	ESelectedAxis IsAxis(UPrimitiveComponent* axis);
	ESelectedAxis GetSelectedAxis() const { return SelectedAxis; }
	void SetPrevMousePos(FVector4 mouse);
	void SetActorXAxis(FVector4 axis);
	void SetActorYAxis(FVector4 axis);
	void SetActorZAxis(FVector4 axis);

	FVector4 GetXAxis() { return actorXAxis; }
	FVector4 GetYAxis() { return actorYAxis; }
	FVector4 GetZAxis() { return actorZAxis; }
private:
	ESelectedAxis SelectedAxis = ESelectedAxis::None;
	TArray<UPrimitiveComponent*> axisComponents;
	virtual const char* GetTypeName() override;

private:
	FVector4 actorXAxis;
	FVector4 actorYAxis;
	FVector4 actorZAxis;
	FVector4 prevMousePos;
	void DoTransform(FTransform& AT, float Result, AActor* Actor);
};

