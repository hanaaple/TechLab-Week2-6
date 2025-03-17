#pragma once
#include "Actor.h"
class ABillboardText : public AActor {
	DECLARE_OBJECT(ABillboard, AActor)
	using Super = AActor;
public:
	ABillboardText();
	virtual ~ABillboardText() = default;
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual const char* GetTypeName() override;
	void SetText(const string NewText);
	string text;
};