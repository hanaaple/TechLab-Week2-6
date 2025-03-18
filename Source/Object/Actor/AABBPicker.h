#pragma once

#include "Actor.h"
#include "Object/PrimitiveComponent/UPrimitiveComponent.h"

class AAABBPicker : public AActor {
	using Super = AActor;
public:
	AAABBPicker();
	~AAABBPicker() = default;

	virtual void Tick(float DeltaTime) override;
	virtual void LateTick(float DeltaTime) override;
	virtual const char* GetTypeName() override;

	FVector RayCast(FVector mouse, ACamera* camera);
	AActor* CheckCollision(FVector rayOrigin, FVector rayDir);
	UPrimitiveComponent* CheckGizmo(FVector rayOrigin, FVector rayDir);
	UTorusComp* CheckRotationGizmo(FVector rayOrigin, FVector rayDir);
	FVector4 clickedPosition;
};