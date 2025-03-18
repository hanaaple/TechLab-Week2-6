#pragma once
#include "Object/Actor/Actor.h"
#include "Object/Gizmo/EditorGizmos.h"
#include "Object/PrimitiveComponent/UPrimitiveComponent.h"
#include "Core/Math/Vector4.h"

class ARotationGizmo : public AActor
{
	DECLARE_OBJECT(ARotationGizmo, AActor)
public:
	ARotationGizmo();
public:
	virtual void Tick(float DeltaTime) override;
	void SetScaleByDistance();
	void SetActorVisibility(bool bNewActive);
	void SetSelectedAxis(ESelectedAxis NewAxis) { SelectedAxis = NewAxis; }
	TArray<UTorusComp*> GetAxis() { return axisComponents; }
	ESelectedAxis IsAxis(UTorusComp* axis);
	ESelectedAxis GetSelectedAxis() const { return SelectedAxis; }
	void SetPrevMousePos(FVector4 mouse);
private:
	ESelectedAxis SelectedAxis = ESelectedAxis::None;
	TArray<UTorusComp*> axisComponents;
	virtual const char* GetTypeName() override;
	void DoTransform(FTransform& AT, float Result, AActor* Actor);
	FVector4 prevMousePos;
};