#pragma once
#include "JsonSavehelper.h"
#include "Core/Engine.h"
#include "Core/Container/Array.h"
#include "Core/Container/Set.h"
#include "Core/Container/String.h"
#include "Core/Rendering/BatchRenderContext.h"
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
	void RenderMainTargets(URenderer& Renderer);

	void ClearWorld();
	void LoadWorld(const char* SceneName);
	void SaveWorld();
	
	void AddRenderComponent(class UPrimitiveComponent* Component)
	{
		RenderComponents.Add(Component);

		Component->SetDirty(true);
	}
	void RemoveRenderComponent(class UPrimitiveComponent* Component)
	{
		RenderComponents.Remove(Component);

		if (Component->GetRenderMode() == Individual)
		{
			IndividualRenders.Remove(Component);
		}
		else if (Component->GetRenderMode() == Batch)
		{
			RemoveFromBatch(Component, Component->GetCurFrameData());
		}
	}
	const TArray<AActor*>& GetActors() const { return Actors; }
	const TArray<UPrimitiveComponent*> GetRenderComponents() const { return RenderComponents; }
private:
	UWorldInfo GetWorldInfo() const;
	bool HasBatchRendersKey(EShaderType ShaderType, ETextureType TextureType, D3D11_PRIMITIVE_TOPOLOGY Topology, bUseIndexBufferFlag bUseIndexBuffer);
	void CheckRemoveMap(EShaderType ShaderType, ETextureType TextureType, D3D11_PRIMITIVE_TOPOLOGY Topology, bUseIndexBufferFlag bUseIndexBuffer, EPrimitiveMeshType MeshType);
	void CheckInitMap(const FRenderData& FrameData);
	void AddToBatch(UPrimitiveComponent* Component);
	void RemoveFromBatch(UPrimitiveComponent* Component, const FRenderData& FrameData);
	
public:
	FString SceneName;
	uint32 Version = 1;
	
protected:
	TArray<AActor*> Actors;
	TArray<AActor*> ActorsToSpawn;	// PlayBegin() Before Tick()
	TArray<AActor*> PendingDestroyActors; // TODO: 추후에 TQueue로 변경
	
	TArray<UPrimitiveComponent*> RenderComponents;	

	
	TArray<UPrimitiveComponent*> IndividualRenders;
	//TArray<UPrimitiveComponent*> InstancingRenders;
	//TMap<UMaterial*, TMap<D3D_PRIMITIVE_TOPOLOGY, TMap<uint8, FBatchRenderContext>>> BatchRenders;
	TMap<EShaderType, TMap<ETextureType, TMap<D3D_PRIMITIVE_TOPOLOGY, TMap<bUseIndexBufferFlag, FBatchRenderContext>>>> BatchRenders;
};

template <typename T>
	requires std::derived_from<T, AActor>
T* UWorld::SpawnActor()
{
	
	if (UWorld* World = UEngine::Get().GetWorld())
	{
		T* Actor = FObjectFactory::ConstructObject<T>();
		Actor->SetWorld(World);
		ActorsToSpawn.Add(Actor);
		return Actor;
	}

	UE_LOG("Actor Construction Failed. World is nullptr");
	return nullptr;
}