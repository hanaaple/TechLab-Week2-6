#pragma once
#include "Core/EngineTypes.h"
#include "Object/UObject.h"

class UActorComponent : public UObject
{
public:
    DECLARE_OBJECT(UActorComponent, UObject)
	UActorComponent() : Super(){
    }

	virtual void BeginPlay();
	virtual void Tick(float DeltaTime);
	virtual void EndPlay(const EEndPlayReason::Type Reason);
	virtual void Activate(){ bIsActive = true;}
	virtual void Deactivate(){ bIsActive = false;}
	
	bool CanEverTick() const { return bCanEverTick; }

	virtual class AActor* GetOwner() const;
	virtual void SetOwner(class AActor* InOwner) { Owner = InOwner; }

protected:
	bool bCanEverTick = true;
	bool bIsActive;
	class AActor* Owner = nullptr;
};

