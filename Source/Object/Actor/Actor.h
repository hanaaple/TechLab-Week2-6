#pragma once
#include "Object/UObject.h"
#include "Core/Math/Transform.h"
#include "Core/Container/Set.h"
#include "Object/ObjectFactory.h"
#include "Object/ActorComponent/ActorComponent.h"

class UWorld;

class AActor : public UObject
{
    DECLARE_OBJECT(AActor, UObject)
	friend class FEditorManager;
public:
	AActor() = default;
	virtual ~AActor() override = default;
public:
	virtual void BeginPlay();
	virtual void Tick(float DeltaTime);
	virtual void LateTick (float DeltaTime); // 렌더 후 호출
	
	virtual void Destroyed();
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason);
	TSet<UActorComponent*>& GetComponents() { return Components; }

	UWorld* GetWorld() const { return World; }
	void SetWorld(UWorld* InWorld) { World = InWorld; }
	
	// Clear, 세이브 X, 
	bool IsGizmoActor() const { return bIsGizmo; }

private:
	virtual void Pick();
	virtual void UnPick();

public:
	template<typename T>
		requires std::derived_from<T, UActorComponent>
	T* AddComponent()
	{
		T* ObjectInstance = FObjectFactory::ConstructObject<T>();
		Components.Add(ObjectInstance);
		ObjectInstance->SetOwner(this);

		return ObjectInstance;
	}

	// delete
	template<typename T>
		requires std::derived_from<T, UActorComponent>
	void RemoveComponent(T* Object)
	{
		Components.Remove(Object);
	}

	/** @return Actor's World Transform */
	FTransform GetActorTransform() const;
	/** @param
	 * * Set Actor World Transform */
	void SetActorTransform(const FTransform& NewTransform);

	// Actor는 World 기준이라 없다? -> 그런데 Actor에 Parent는 있을 수 있다?
	/** @param NewRelativeTransform 새로운 상대 좌표 */
	void SetActorRelativeTransform(const FTransform& NewRelativeTransform);
	
	bool CanEverTick() const { return bCanEverTick; }
	virtual const char* GetTypeName();

	bool Destroy();

public:
	USceneComponent* GetRootComponent() const { return RootComponent; }
	virtual void SetRootComponent(USceneComponent* InRootComponent) { RootComponent = InRootComponent; }

public:
	void SetColor(FVector4 InColor);
	void SetUseVertexColor(bool bUseVertexColor);

protected:
	bool bCanEverTick = true;
	USceneComponent* RootComponent = nullptr;
	bool bIsGizmo = false;

private:
	UWorld* World = nullptr;
	TSet<UActorComponent*> Components;
};

