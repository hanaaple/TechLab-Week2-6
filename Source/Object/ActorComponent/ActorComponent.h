﻿#pragma once
#include "Core/EngineTypes.h"
#include "Object/UObject.h"

class UActorComponent : public UObject
{
public:
    DECLARE_OBJECT(UActorComponent, UObject)
	UActorComponent() = default;
	
	virtual void BeginPlay();
	virtual void Tick(float DeltaTime);
	virtual void EndPlay(const EEndPlayReason::Type Reason);
	virtual void Activate(){ bIsActive = true;}
	virtual void Deactivate(){ bIsActive = false;}

	bool GetIsActive() const { return bIsActive;}	
	bool CanEverTick() const { return bCanEverTick; }

	virtual class AActor* GetOwner() const;
	virtual void SetOwner(AActor* InOwner);

protected:
	bool bCanEverTick = true;
	bool bIsActive;
	AActor* Owner = nullptr;
};

