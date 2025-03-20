#pragma once
#include "Actor.h"

class UTextComp;

class ABillboardText : public AActor {
	DECLARE_OBJECT(ABillboard, AActor)
	using Super = AActor;
public:
	ABillboardText();
	virtual ~ABillboardText() = default;
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual const char* GetTypeName() override;

	void SetText(FName NewText);

public:
	USceneComponent* FollowComponent = nullptr;
	FVector Offset = FVector(0, 0, 2);
private:
	UTextComp* TextComp = nullptr;
};
