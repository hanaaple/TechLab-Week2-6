#include "World.h"
#include <cassert>
#include "JsonSavehelper.h"

#include "Core/Container/Map.h"
#include "Core/Input/PlayerInput.h"
#include "Object/Actor/Arrow.h"
#include "Object/Actor/Camera.h"

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
	// 렌더링 되는 컴포넌트들
	// for (auto proxy : )
	// {
	// 	if (proxy->IsDirty)
	// 	{
	// 		// visible change
	// 		// 
	// 		proxy 기반 변경
	// 	}
	// }
	// 변경사항이 있다면
	// 배치, 인스턴싱, 별개 렌더링 Array 업데이트
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
	
	// if (APlayerInput::Get().GetMouseDown(false))
	// {
	// 	RenderPickingTexture(*Renderer);
	// }
	
	RenderMainTargets(*Renderer);

	
	// DisplayPickingTexture(*Renderer);

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
	
	//for (const auto& MaterialMapped : BatchRenders)
	{
		//for (const auto& TopologyMapped : MaterialMapped.Value)
		// for (auto& TopologyMapped : BatchRenders)
		// {
		// 	for (auto& [_, BatchContext] : TopologyMapped.Value)
		// 	{
		// 		DrawBatch(Renderer, BatchContext);
		// 	}
		// }
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
	
	// 셰이더 변경 불가
	Renderer.PrepareMain();
	Renderer.PrepareMainShader();
	for (auto* RenderTarget : IndividualRenders)
	{
		//Renderer.PrepareMainShader();
		// RenderTarget의 Material로 Set
		
		if (RenderTarget->GetDepth() > 0)
		{
			continue;
		}
		RenderTarget->Render();
	}
	

	Renderer.PrepareZIgnore();
	for (auto& RenderComponent: ZIgnoreRenderComponents)
	{
		uint32 depth = RenderComponent->GetDepth();
		RenderComponent->Render();
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

void UWorld::DrawBatch(URenderer& Renderer)
{
	//FBatchRenderContext& BatchRenderContext
	//Renderer.RenderBatch(BatchRenderContext);
}

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

void UWorld::AddZIgnoreComponent(UPrimitiveComponent* InComponent)
{
	ZIgnoreRenderComponents.Add(InComponent);
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