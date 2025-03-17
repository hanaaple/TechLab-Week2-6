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

enum class EGizmoType : uint8
{
	Translate,
	Rotate,
	Scale,
	Max
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
	TArray<UCylinderComp*> GetAxis() { return axisComponents; }
	ESelectedAxis IsAxis(UCylinderComp* axis);
	ESelectedAxis GetSelectedAxis() const { return SelectedAxis; }
	EGizmoType GetGizmoType() const { return GizmoType; }
	void SetPrevMousePos(FVector4 mouse);
	void SetActorXAxis(FVector4 axis);
	void SetActorYAxis(FVector4 axis);
	void SetActorZAxis(FVector4 axis);

private:
	ESelectedAxis SelectedAxis = ESelectedAxis::None;
	EGizmoType GizmoType = EGizmoType::Translate;
	TArray<UCylinderComp*> axisComponents;
	virtual const char* GetTypeName() override;

private:
	FVector4 actorXAxis;
	FVector4 actorYAxis;
	FVector4 actorZAxis;
	FVector4 prevMousePos;
	FVector4 curMousePos;
	void DoTransform(FTransform& AT, float Result, AActor* Actor);
};

