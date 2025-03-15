#pragma once
#include "Object/Actor/Actor.h"

class AAxis : public AActor
{
	DECLARE_OBJECT(AAxis, AActor)

	using Super = AActor;
public:
	AAxis();
	virtual ~AAxis() = default;

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual const char* GetTypeName() override;
};

