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

	ESelectedAxis IsAxis(UCylinderComp* axis);
	
	void SetScaleByDistance();
	void SetActorVisibility(bool bNewActive) { 	if (RootComponent != nullptr) RootComponent->SetVisibility(bNewActive); }
	void SetSelectedAxis(ESelectedAxis NewAxis) { SelectedAxis = NewAxis; }
	TArray<UCylinderComp*> GetAxis() { return axisComponents; }
	ESelectedAxis GetSelectedAxis() const { return SelectedAxis; }
	EGizmoType GetGizmoType() const { return GizmoType; }

	void SetPrevMousePos(FVector mouse) { prevMousePos = mouse; }
	void SetActorXAxis(FVector axis) { actorXAxis = axis; }
	void SetActorYAxis(FVector axis) { actorYAxis = axis; }
	void SetActorZAxis(FVector axis) { actorZAxis = axis; }

    FVector GetXAxis() const { return actorXAxis; }
	FVector GetYAxis() const { return actorYAxis; }
	FVector GetZAxis() const { return actorZAxis; }
private:
	ESelectedAxis SelectedAxis = ESelectedAxis::None;
	EGizmoType GizmoType = EGizmoType::Translate;
	TArray<UCylinderComp*> axisComponents;
	virtual const char* GetTypeName() override;

private:
	FVector actorXAxis;
	FVector actorYAxis;
	FVector actorZAxis;
	FVector prevMousePos;
	void DoTransform(FTransform& AT, float Result, AActor* Actor);
};

