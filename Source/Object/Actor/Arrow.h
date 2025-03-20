#pragma once
#include "Object/Actor/Actor.h"

class AArrow : public AActor
{
    DECLARE_OBJECT(AArrow, AActor)
	using Super = AActor;

public:
	AArrow();
	virtual ~AArrow() = default;
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual const char* GetTypeName() override;
};

