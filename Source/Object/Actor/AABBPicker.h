#pragma once

#include "Actor.h"

class AAABBPicker : public AActor {
	using Super = AActor;
public:
	AAABBPicker();
	~AAABBPicker() = default;

	virtual void Tick(float DeltaTime) override;
	virtual void LateTick(float DeltaTime) override;
	virtual const char* GetTypeName() override;

	AActor* CheckCollision(FVector rayOrigin, FVector rayDir);
};