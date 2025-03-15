#pragma once
#include "JsonSavehelper.h"
#include "Core/Engine.h"
#include "Core/Container/Array.h"
#include "Core/Container/Set.h"
#include "Core/Container/String.h"
#include "Object/UObject.h"
#include "Object/ObjectFactory.h"

#include "Object/Actor/Arrow.h"
#include "Object/Actor/Picker.h"
#include "Object/PrimitiveComponent/UPrimitiveComponent.h"

class AActor;


class UWorld :public UObject
{
public:
	UWorld() = default;
	virtual ~UWorld() = default;

public:
	void BeginPlay();
	void Tick(float DeltaTime);
	void LateTick(float DeltaTime);
	void UpdateRenderComponents();

	template <typename T>
		requires std::derived_from<T, AActor>
	T* SpawnActor();
  
	bool DestroyActor(AActor* InActor);
	
	void Render();
	//void RenderPickingTexture(URenderer& Renderer);
	//void DisplayPickingTexture(URenderer& Renderer);
	void DrawBatch(URenderer& Renderer);
	//void DrawBatch(URenderer& Renderer, FBatchRenderContext& BatchRenderContext);
	void RenderMainTargets(URenderer& Renderer);

	void ClearWorld();
	void LoadWorld(const char* SceneName);
	void SaveWorld();

	void AddZIgnoreComponent(UPrimitiveComponent* InComponent);
	void RemoveZIgnoreComponent(UPrimitiveComponent* InComponent) {ZIgnoreRenderComponents.Remove(InComponent); }
	bool ContainsZIgnoreComponent(UPrimitiveComponent* InComponent) {return ZIgnoreRenderComponents.Find(InComponent) != -1; }
	
	void AddRenderComponent(class UPrimitiveComponent* Component);
	void RemoveRenderComponent(class UPrimitiveComponent* Component) { RenderComponents.Remove(Component); }
	const TArray<AActor*>& GetActors() const { return Actors; }
private:
	UWorldInfo GetWorldInfo() const;

public:
	FString SceneName;
	uint32 Version = 1;
	
protected:
	TArray<AActor*> Actors;
	TArray<AActor*> ActorsToSpawn;	// PlayBegin() Before Tick()
	TArray<AActor*> PendingDestroyActors; // TODO: 추후에 TQueue로 변경
	
	TArray<UPrimitiveComponent*> RenderComponents;
	TArray<UPrimitiveComponent*> ZIgnoreRenderComponents;	
	TArray<UPrimitiveComponent*> IndividualRenders;
	
	//TArray<UPrimitiveComponent*> InstancingRenders;
	//TMap<UMaterial*, TMap<D3D_PRIMITIVE_TOPOLOGY, TMap<uint8, FBatchRenderContext>>> BatchRenders;
	//TMap<D3D_PRIMITIVE_TOPOLOGY, TMap<uint8, FBatchRenderContext>> BatchRenders;
};

template <typename T>
	requires std::derived_from<T, AActor>
T* UWorld::SpawnActor()
{
	T* Actor = FObjectFactory::ConstructObject<T>();
	
	if (UWorld* World = UEngine::Get().GetWorld())
	{
		Actor->SetWorld(World);
		Actors.Add(Actor);
		ActorsToSpawn.Add(Actor);
		return Actor;
	}

	UE_LOG("Actor Construction Failed. World is nullptr");
	return nullptr;
}