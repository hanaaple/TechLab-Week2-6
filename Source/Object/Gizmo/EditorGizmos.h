#pragma once
#include "Object/Actor/Actor.h"
#include "Object/PrimitiveComponent/UPrimitiveComponent.h"

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
public:
	AEditorGizmos();

public:
	virtual void Tick(float DeltaTime) override;
	void SetScaleByDistance();
	void SetActorVisibility(bool bNewActive);
	void SetSelectedAxis(ESelectedAxis NewAxis) { SelectedAxis = NewAxis; }
	ESelectedAxis GetSelectedAxis() const { return SelectedAxis; }
	EGizmoType GetGizmoType() const { return GizmoType; }
	
private:
	ESelectedAxis SelectedAxis = ESelectedAxis::None;
	EGizmoType GizmoType = EGizmoType::Translate;

	virtual const char* GetTypeName() override;

private:
	void DoTransform(FTransform& AT, FVector Result, AActor* Actor);
};

