#include "World.h"
#include <cassert>
#include "JsonSavehelper.h"

#include "Core/Container/Map.h"
#include "Core/Input/PlayerInput.h"
#include "Object/Actor/Camera.h"

#include "Object/Actor/Cone.h"
#include "Object/Actor/Cube.h"
#include "Object/Actor/Cylinder.h"
#include "Object/Actor/Sphere.h"
#include "Object/PrimitiveComponent/UPrimitiveComponent.h"
#include "Static/FEditorManager.h"
#include <Object/Actor/BillBoardText.h>


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
		Actors.Add(Actor);
	}
	ActorsToSpawn.Empty();
	
	for (const auto& Actor : Actors)
	{
		Actor->ActivateComponent();
	}

	const auto CopyActors = Actors;
	for (const auto& Actor : CopyActors)
	{
		if (Actor->CanEverTick())
		{
			Actor->Tick(DeltaTime);
		}
	}

	if (FEditorManager::Get().GetSelectedActor() != nullptr) {
		ACamera* cam = FEditorManager::Get().GetCamera();
		if (!cam->GetIsMoving() && APlayerInput::Get().GetKeyDown(EKeyCode::F))
		{
			cam->SetOriginalRotation();
			cam->SetIsMoving(true);
		}
		if (cam->GetIsMoving()) {
			FTransform camTransform = cam->GetActorTransform();
			FVector actorPos = FEditorManager::Get().GetSelectedActor()->GetActorTransform().GetPosition();
			FVector camPos = camTransform.GetPosition();
			FVector Direction = ((actorPos - FVector(5, 0, 0)) - camPos);
			float distance = Direction.Length();
			Direction = Direction.GetSafeNormal();
			if (actorPos.X - camPos.X > 5.01f) {
				camTransform.SetPosition(camPos + Direction * (distance/10.0f));
				if (camTransform.GetEulerRotation().Length() > 0.1f) {
					camTransform.SetRotation(camTransform.GetEulerRotation() - cam->GetOriginalRotation() * 0.02f);
				}
				cam->SetActorTransform(camTransform);
			}
			else {
				cam->SetIsMoving(false);
			}
		}
		if (APlayerInput::Get().GetMouseDown(true)) {
			cam->SetIsMoving(false);
		}
	}
	else {
		FEditorManager::Get().GetCamera()->SetIsMoving(false);
	}
	


	if (APlayerInput::Get().GetKeyDown(EKeyCode::Space))
	{
		int type = static_cast<int>(FEditorManager::Get().GetGizmoType());
		type = (type + 1) % static_cast<int>(EGizmoType::Max);
		FEditorManager::Get().SetGizmoType(static_cast<EGizmoType>(type));
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
			// 이전 프레임에 Batch인 경우 -> 배치에서 삭제
			if (PrevFrameData.RenderMode == Batch)
			{
				RemoveFromBatch(RenderComponent, PrevFrameData);
			}
			else if (PrevFrameData.RenderMode == Individual)
			{
				IndividualRenders.Remove(RenderComponent);
			}

			// Batch가 된 경우 -> 추가
			if (CurFrameData.RenderMode == Batch)
			{
				AddToBatch(RenderComponent);
			}
			else if (CurFrameData.RenderMode == Individual)
			{
				IndividualRenders.Add(RenderComponent);
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

	// 굳이 여기서 Shader, Texture, Topology를 안해줘도 내부적으로 동일한 경우 변경이 안되어 괜찮다.
	for (auto& [ShaderType, ShaderMapped] : BatchRenders)
	{
		for (auto& [Texture, TextureMapped] : ShaderMapped)
		{
			for (auto& TopologyMapped : TextureMapped)
			{
				for (auto& [_, BatchContext] : TopologyMapped.Value)
				{
					Renderer.RenderBatch(BatchContext);
				}
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
	for (auto* RenderTarget : IndividualRenders)
	{
		if (RenderTarget != nullptr) {
			// Texture 변경
			if (RenderTarget->GetDepth() > 0)
			{
				ZIgnoreRenderComponents.Add(RenderTarget);
				continue;
			}
			RenderTarget->Render();
		}
	}

	Renderer.PrepareZIgnore();
	for (auto& RenderTarget: ZIgnoreRenderComponents)
	{
		RenderTarget->Render();
	}
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
		else if (ObjectInfo->ObjectType == "BillboardText")
		{
			Actor = SpawnActor<ABillboardText>();
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

bool UWorld::HasBatchRendersKey(EShaderType ShaderType, ETextureType TextureType, D3D11_PRIMITIVE_TOPOLOGY Topology, bUseIndexBufferFlag bUseIndexBuffer)
{
	if (BatchRenders.Contains(ShaderType))
	{
		if (BatchRenders[ShaderType].Contains(TextureType))
		{
			if (BatchRenders[ShaderType][TextureType].Contains(Topology))
			{
				if (BatchRenders[ShaderType][TextureType][Topology].Contains(bUseIndexBuffer))
				{
					return true;
				}
			}
		}
	}
	
	return false;
}

void UWorld::CheckRemoveMap(EShaderType ShaderType, ETextureType TextureType, D3D11_PRIMITIVE_TOPOLOGY Topology, bUseIndexBufferFlag bUseIndexBuffer, EPrimitiveMeshType MeshType)
{
	// TODO 더이상 접근하지 않는 캐싱된 배치 렌더링 타겟 해제
	// BufferCache->CheckRemove(BatchContext.Texture, BatchContext.Topology, VertexBuffer);
	if (BatchRenders.Contains(ShaderType))
	{
		if (BatchRenders[ShaderType].Contains(TextureType))
		{
			if (BatchRenders[ShaderType][TextureType].Contains(Topology))
			{
				if (BatchRenders[ShaderType][TextureType][Topology].Contains(bUseIndexBuffer))
				{
					if (BatchRenders[ShaderType][TextureType][Topology][bUseIndexBuffer].RenderComponentMap.Contains(MeshType))
					{
						if (BatchRenders[ShaderType][TextureType][Topology][bUseIndexBuffer].RenderComponentMap[MeshType].Num() == 0)
						{
							BatchRenders[ShaderType][TextureType][Topology][bUseIndexBuffer].RenderComponentMap.Remove(MeshType);
						}
					}
				
					if (BatchRenders[ShaderType][TextureType][Topology][bUseIndexBuffer].RenderComponentMap.Num() == 0)
					{
						// Remove Cached Buffer
						BatchRenders[ShaderType][TextureType][Topology].Remove(bUseIndexBuffer);
					}
				}

				if (BatchRenders[ShaderType][TextureType][Topology].Num() == 0)
				{
					BatchRenders[ShaderType][TextureType].Remove(Topology);
				}
			}
			if (BatchRenders[ShaderType][TextureType].Num() == 0)
			{
				BatchRenders[ShaderType].Remove(TextureType);
			}
		}
		if (BatchRenders[ShaderType].Num() == 0)
		{
			BatchRenders.Remove(ShaderType);
		}
	}
}

void UWorld::CheckInitMap(const FRenderData& FrameData)
{
	EShaderType ShaderType = FrameData.ShaderType;
	ETextureType TextureType = FrameData.TextureType;
	D3D11_PRIMITIVE_TOPOLOGY Topology = FrameData.Topology;
	bUseIndexBufferFlag bUseIndexBuffer = MeshResourceCache::Get().HasIndexData(FrameData.MeshType);
	EPrimitiveMeshType MeshType = FrameData.MeshType;
	
	if (!BatchRenders.Contains(ShaderType))
	{
		BatchRenders.Add(ShaderType, TMap<ETextureType, TMap<D3D_PRIMITIVE_TOPOLOGY, TMap<bUseIndexBufferFlag, FBatchRenderContext>>>());
		if (!BatchRenders[ShaderType].Contains(TextureType))
		{
			BatchRenders[ShaderType].Add(TextureType, TMap<D3D_PRIMITIVE_TOPOLOGY, TMap<bUseIndexBufferFlag, FBatchRenderContext>>());
			if (!BatchRenders[ShaderType][TextureType].Contains(Topology))
			{
				BatchRenders[ShaderType][TextureType].Add(Topology, TMap<bUseIndexBufferFlag, FBatchRenderContext>());
				if (!BatchRenders[ShaderType][TextureType][Topology].Contains(bUseIndexBuffer))
				{
					BatchRenders[ShaderType][TextureType][Topology].Add(bUseIndexBuffer, FBatchRenderContext());

					if (!BatchRenders[ShaderType][TextureType][Topology][bUseIndexBuffer].RenderComponentMap.Contains(MeshType)){
						BatchRenders[ShaderType][TextureType][Topology][bUseIndexBuffer].RenderComponentMap.Add(MeshType, TArray<UPrimitiveComponent*>());
					}
				}
			}
		}
	}
}

void UWorld::AddToBatch(UPrimitiveComponent* Component)
{
	const auto& CurFrameData = Component->GetCurFrameData();
		
	CheckInitMap(CurFrameData);

	bUseIndexBufferFlag useIndexBuffer = MeshResourceCache::Get().HasIndexData(CurFrameData.MeshType);
	auto& CurBatchRenderContext = BatchRenders[CurFrameData.ShaderType][CurFrameData.TextureType][CurFrameData.Topology][useIndexBuffer];
	CurBatchRenderContext.bIsDirty = true;
	CurBatchRenderContext.ShaderType = CurFrameData.ShaderType;
	CurBatchRenderContext.TextureType = CurFrameData.TextureType;
	CurBatchRenderContext.Topology = CurFrameData.Topology;
	CurBatchRenderContext.bUseIndexBuffer = useIndexBuffer;
	CurBatchRenderContext.RenderComponentMap[CurFrameData.MeshType].Add(Component);
}

void UWorld::RemoveFromBatch(UPrimitiveComponent* Component, const FRenderData& FrameData)
{
	bUseIndexBufferFlag useIndexBuffer = MeshResourceCache::Get().HasIndexData(FrameData.MeshType);
	// 이전에 렌더링 되었는지, 데이터 여부로 확인
	if (HasBatchRendersKey(FrameData.ShaderType, FrameData.TextureType, FrameData.Topology, useIndexBuffer))
	{
		auto& PrevBatchRenderContext = BatchRenders[FrameData.ShaderType][FrameData.TextureType][FrameData.Topology][useIndexBuffer];
		PrevBatchRenderContext.bIsDirty = true;
				
		if (PrevBatchRenderContext.RenderComponentMap.Contains(FrameData.MeshType))
		{
			PrevBatchRenderContext.RenderComponentMap[FrameData.MeshType].Remove(Component);
		}

		// 빈 Map 삭제
		CheckRemoveMap(FrameData.ShaderType, FrameData.TextureType, FrameData.Topology, useIndexBuffer, FrameData.MeshType);
	}
}
