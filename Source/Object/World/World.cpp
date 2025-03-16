#include "World.h"
#include <cassert>
#include "JsonSavehelper.h"

#include "Core/Container/Map.h"
#include "Core/Input/PlayerInput.h"
#include "Object/Actor/Camera.h"
#include <Object/Gizmo/EditorGizmos.h>
#include <Object/Actor/ABoundingBox.h>

#include "Object/Actor/Cone.h"
#include "Object/Actor/Cube.h"
#include "Object/Actor/Cylinder.h"
#include "Object/Actor/Sphere.h"
#include "Object/PrimitiveComponent/UPrimitiveComponent.h"
#include "Static/FEditorManager.h"


void UWorld::BeginPlay()
{
	for (const auto& Actor : Actors)
	{
		Actor->BeginPlay();
		if (ActorsToSpawn.Find(Actor) != -1)
		{
			ActorsToSpawn.Remove(Actor);
		}
	}
}

void UWorld::Tick(float DeltaTime)
{
	for (const auto& Actor : Actors)
	{
		Actor->ActivateComponent();
	}
	
	for (const auto& Actor : ActorsToSpawn)
	{
		Actor->BeginPlay();
	}
	ActorsToSpawn.Empty();

	const auto CopyActors = Actors;
	for (const auto& Actor : CopyActors)
	{
		if (Actor->CanEverTick())
		{
			Actor->Tick(DeltaTime);
		}
	}
}

void UWorld::LateTick(float DeltaTime)
{
	const auto CopyActors = Actors;
	for (const auto& Actor : CopyActors)
	{
		if (Actor->CanEverTick())
		{
			Actor->LateTick(DeltaTime);
		}
	}

	for (const auto& PendingActor : PendingDestroyActors)
	{
		// Engine에서 제거
		UEngine::Get().GObjects.Remove(PendingActor->GetUUID());
	}
	PendingDestroyActors.Empty();
}

void UWorld::UpdateRenderComponents()
{
	for (auto* RenderComponent : RenderComponents)
	{
		const auto& PrevFrameData = RenderComponent->GetPrevFrameData();
		const auto& CurFrameData = RenderComponent->GetCurFrameData();
		if(RenderComponent->GetIsDirty())
		{
			if (PrevFrameData.RenderMode == Batch)
			{
				if (HasBatchRendersKey(PrevFrameData.Texture, PrevFrameData.Topology, PrevFrameData.bUseIndexBuffer))
				{
					auto& PrevBatchRenderContext = BatchRenders[PrevFrameData.Texture][PrevFrameData.Topology][PrevFrameData.bUseIndexBuffer];
					PrevBatchRenderContext.bIsDirty = true;
				
					if (PrevBatchRenderContext.RenderComponentMap.Contains(PrevFrameData.MeshType))
					{
						PrevBatchRenderContext.RenderComponentMap[PrevFrameData.MeshType].Remove(RenderComponent);
					}
				
					CheckRemoveMap(PrevFrameData.Texture, PrevFrameData.Topology, PrevFrameData.bUseIndexBuffer, PrevFrameData.MeshType);
				}
			}

			if (CurFrameData.RenderMode == Batch)
			{
				CheckInitMap(CurFrameData.Texture, CurFrameData.Topology, CurFrameData.bUseIndexBuffer, CurFrameData.MeshType);
						
				auto& CurBatchRenderContext = BatchRenders[CurFrameData.Texture][CurFrameData.Topology][CurFrameData.bUseIndexBuffer];
				CurBatchRenderContext.bIsDirty = true;
				CurBatchRenderContext.RenderComponentMap[CurFrameData.MeshType].Add(RenderComponent);
			}
			
			RenderComponent->SetDirty(false);
		}
	}
}


void UWorld::Render()
{
	URenderer* Renderer = UEngine::Get().GetRenderer();

	if (Renderer == nullptr)
	{
		return;
	}

	ACamera* cam = FEditorManager::Get().GetCamera();
	Renderer->UpdateViewMatrix(cam->GetActorTransform());
	Renderer->UpdateProjectionMatrix(cam);
	
	RenderMainTargets(*Renderer);
}

// void UWorld::RenderPickingTexture(URenderer& Renderer)
// {
// 	// Renderer.PreparePicking();
// 	// Renderer.PreparePickingShader();
// 	//
// 	// // for (auto& [MeshType, RenderComponents] : RenderComponentTable)
// 	// {
// 	// 	TArray<UPrimitiveComponent*> BatchTargetComponents;
// 	// 	for (auto RenderComponent : RenderComponents)
// 	// 	{
// 	// 		//TODO11
// 	// 		// Picking들도 정상적으로 렌더링 시켜야됨. 
// 	// 		if (RenderComponent->GetIsBatch())
// 	// 		{
// 	// 			BatchTargetComponents.Add(RenderComponent);
// 	// 		}
// 	// 		else
// 	// 		{
// 	// 			if (RenderComponent->GetOwner()->GetDepth() > 0)
// 	// 			{
// 	// 				continue;
// 	// 			}
// 	// 			uint32 UUID = RenderComponent->GetUUID();
// 	// 			RenderComponent->UpdateConstantPicking(Renderer, APicker::EncodeUUID(UUID));
// 	// 			RenderComponent->Render();
// 	// 		}
// 	// 	}
// 	// 	//DrawPickingBatch(BatchTargetComponents);
// 	// }
// 	//
// 	// Renderer.PrepareZIgnore();
// 	// for (auto& RenderComponent: ZIgnoreRenderComponents)
// 	// {
// 	// 	uint32 UUID = RenderComponent->GetUUID();
// 	// 	RenderComponent->UpdateConstantPicking(Renderer, APicker::EncodeUUID(UUID));
// 	// 	uint32 depth = RenderComponent->GetDepth();
// 	// 	RenderComponent->Render();
// 	// }
// }

void UWorld::RenderMainTargets(URenderer& Renderer)
{
	// Depth Stencil, RenderTarget, BlendState 변경
	Renderer.PrepareMain();
	Renderer.PrepareMainShader();	// 우선 Shader 1개로만
	
	for (auto& [Texture, TextureMapped] : BatchRenders)
	{
		for (auto& TopologyMapped : TextureMapped)
		{
			for (auto& [_, BatchContext] : TopologyMapped.Value)
			{
				Renderer.PrepareTexture(BatchContext.Texture);
				Renderer.RenderBatch(BatchContext);
			}
		}
	}

	//for (const auto& MaterialMapped : InstancingRenders)
	{
		// TODO 인스턴싱은 예정!
		// for (const auto& TopologyMapped : MaterialMapped.Value)
		// {
		// 	for (auto [_, Context] : TopologyMapped.Value)
		// 	{
		// 		DrawInstancing(Context);
		// 	}
		// }
	}

	TArray<UPrimitiveComponent*> ZIgnoreRenderComponents;
	for (auto* RenderTarget : RenderComponents)
	{
		Renderer.PrepareTexture(RenderTarget->GetTexture());
		// Texture 변경
		if (RenderTarget->GetDepth() > 0)
		{
			ZIgnoreRenderComponents.Add(RenderTarget);
			continue;
		}
		RenderTarget->Render();
	}
	

	Renderer.PrepareZIgnore();
	for (auto& RenderTarget: ZIgnoreRenderComponents)
	{
		Renderer.PrepareTexture(RenderTarget->GetTexture());
		RenderTarget->Render();
	}
	// 1. 같은 메쉬여도 배치 여부가 다를수 있다.
	// 2. 다른 메쉬여도 같은 토폴로지, 같은 머터리얼과 셰이더, 트

	// 같은 토폴로지, 셰이더, 다른 메시 -> 배치 렌더링
	// 같은 토폴로지, 셰이더, 같은 메시 -> 인스턴싱
	
}

// void UWorld::DisplayPickingTexture(URenderer& Renderer)
// {
// 	Renderer.RenderPickingTexture();
// }

void UWorld::ClearWorld()
{
	TArray CopyActors = Actors;
	for (AActor* Actor : CopyActors)
	{
		if (!Actor->IsGizmoActor())
		{
			DestroyActor(Actor);
		}
	}

	UE_LOG("Clear World");
}


bool UWorld::DestroyActor(AActor* InActor)
{
	// 나중에 Destroy가 실패할 일이 있다면 return false; 하기
	assert(InActor);

	if (PendingDestroyActors.Find(InActor) != -1)
	{
		return true;
	}

	// 삭제될 때 Destroyed 호출
	InActor->Destroyed();

	// World에서 제거
	Actors.Remove(InActor);

	// 제거 대기열에 추가
	PendingDestroyActors.Add(InActor);
	return true;
}

void UWorld::AddRenderComponent(UPrimitiveComponent* Component)
{
	RenderComponents.Add(Component);
}


void UWorld::SaveWorld()
{
	const UWorldInfo& WorldInfo = GetWorldInfo();
	JsonSaveHelper::SaveScene(WorldInfo);
}

void UWorld::LoadWorld(const char* SceneName)
{
	if (SceneName == nullptr || strcmp(SceneName, "") == 0){
		return;
	}
	
	UWorldInfo* WorldInfo = JsonSaveHelper::LoadScene(SceneName);
	if (WorldInfo == nullptr) return;

	ClearWorld();

	Version = WorldInfo->Version;
	this->SceneName = WorldInfo->SceneName;
	uint32 ActorCount = WorldInfo->ActorCount;

	// Type 확인
	for (uint32 i = 0; i < ActorCount; i++)
	{
		UObjectInfo* ObjectInfo = WorldInfo->ObjctInfos[i];
		FTransform Transform = FTransform(ObjectInfo->Location, FVector(), ObjectInfo->Scale);
		Transform.Rotate(ObjectInfo->Rotation);

		AActor* Actor = nullptr;
		
		if (ObjectInfo->ObjectType == "Actor")
		{
			Actor = SpawnActor<AActor>();
		}
		else if (ObjectInfo->ObjectType == "Sphere")
		{
			Actor = SpawnActor<ASphere>();
		}
		else if (ObjectInfo->ObjectType == "Cube")
		{
			Actor = SpawnActor<ACube>();
		}
		else if (ObjectInfo->ObjectType == "Arrow")
		{
			Actor = SpawnActor<AArrow>();
		}
		else if (ObjectInfo->ObjectType == "Cylinder")
		{
			Actor = SpawnActor<ACylinder>();
		}
		else if (ObjectInfo->ObjectType == "Cone")
		{
			Actor = SpawnActor<ACone>();
		}
		
		Actor->SetActorTransform(Transform);
	}
}

UWorldInfo UWorld::GetWorldInfo() const
{
	UWorldInfo WorldInfo;
	WorldInfo.ActorCount = Actors.Num();
	WorldInfo.ObjctInfos = new UObjectInfo*[WorldInfo.ActorCount];
	WorldInfo.SceneName = *SceneName;
	WorldInfo.Version = 1;
	uint32 i = 0;
	for (auto& actor : Actors)
	{
		if (actor->IsGizmoActor())
		{
			WorldInfo.ActorCount--;
			continue;
		}
		WorldInfo.ObjctInfos[i] = new UObjectInfo();
		const FTransform& Transform = actor->GetActorTransform();
		WorldInfo.ObjctInfos[i]->Location = Transform.GetPosition();
		WorldInfo.ObjctInfos[i]->Rotation = Transform.GetEulerRotation();
		WorldInfo.ObjctInfos[i]->Scale = Transform.GetScale();
		WorldInfo.ObjctInfos[i]->ObjectType = actor->GetTypeName();

		WorldInfo.ObjctInfos[i]->UUID = actor->GetUUID();
		i++;
	}
	return WorldInfo;
}

bool UWorld::HasBatchRendersKey(ID3D11ShaderResourceView* Texture, D3D11_PRIMITIVE_TOPOLOGY Topology, bUseIndexBufferFlag bUseIndexBuffer)
{
	if (BatchRenders.Contains(Texture))
	{
		if (BatchRenders[Texture].Contains(Topology))
		{
			if (BatchRenders[Texture][Topology].Contains(bUseIndexBuffer))
			{
				return true;
			}
		}
	}
	
	return false;
}

void UWorld::CheckRemoveMap(ID3D11ShaderResourceView* Texture, D3D11_PRIMITIVE_TOPOLOGY Topology, bUseIndexBufferFlag bUseIndexBuffer, EPrimitiveMeshType MeshType)
{
	if (BatchRenders.Contains(Texture))
	{
		if (BatchRenders[Texture].Contains(Topology))
		{
			if (BatchRenders[Texture][Topology].Contains(bUseIndexBuffer))
			{
				if (BatchRenders[Texture][Topology][bUseIndexBuffer].RenderComponentMap.Contains(MeshType))
				{
					if (BatchRenders[Texture][Topology][bUseIndexBuffer].RenderComponentMap[MeshType].Num() == 0)
					{
						BatchRenders[Texture][Topology][bUseIndexBuffer].RenderComponentMap.Remove(MeshType);
					}
				}
				
				if (BatchRenders[Texture][Topology][bUseIndexBuffer].RenderComponentMap.Num() == 0)
				{
					// Remove Cached Buffer
					BatchRenders[Texture][Topology].Remove(bUseIndexBuffer);
				}
			}

			if (BatchRenders[Texture][Topology].Num() == 0)
			{
				BatchRenders[Texture].Remove(Topology);
			}
		}
		if (BatchRenders[Texture].Num() == 0)
		{
			BatchRenders.Remove(Texture);
		}
	}
}

void UWorld::CheckInitMap(ID3D11ShaderResourceView* Texture, D3D11_PRIMITIVE_TOPOLOGY Topology, bUseIndexBufferFlag bUseIndexBuffer, EPrimitiveMeshType MeshType)
{
	if (!BatchRenders.Contains(Texture))
	{
		BatchRenders.Add(Texture, TMap<D3D_PRIMITIVE_TOPOLOGY, TMap<bUseIndexBufferFlag, FBatchRenderContext>>());
		if (!BatchRenders[Texture].Contains(Topology))
		{
			BatchRenders[Texture].Add(Topology, TMap<bUseIndexBufferFlag, FBatchRenderContext>());
			if (!BatchRenders[Texture][Topology].Contains(bUseIndexBuffer))
			{
				BatchRenders[Texture][Topology].Add(bUseIndexBuffer, FBatchRenderContext());

				if (!BatchRenders[Texture][Topology][bUseIndexBuffer].RenderComponentMap.Contains(MeshType)){
					BatchRenders[Texture][Topology][bUseIndexBuffer].RenderComponentMap.Add(MeshType, TArray<UPrimitiveComponent*>());
				}
			}
		}
	}
}
