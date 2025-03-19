#include "UPrimitiveComponent.h"

#include <format>

#include "Object/World/World.h"
#include "DataTypes/Structs.h"
#include "Object/Actor/BillBoardText.h"

void UPrimitiveComponent::Activate()
{
	Super::Activate();

	GetOwner()->GetWorld()->AddRenderComponent(this);

	if (!GetOwner()->IsGizmoActor())
	{
		ABillboardText* Billboard = GetOwner()->GetWorld()->SpawnActor<ABillboardText>();
		Billboard->SetText(FName(std::format("UUID: {}", GetUUID())));
		Billboard->FollowComponent = this;
	}
}

void UPrimitiveComponent::Deactivate()
{
	Super::Deactivate();

	UWorld* World = GetOwner()->GetWorld();
	
	World->RemoveRenderComponent(this);
}

void UPrimitiveComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UPrimitiveComponent::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	TArray<FVertexSimple> VertexData;
	if (TryGetVertexData(&VertexData))
	{
		aabb.UpdateAABB(VertexData);
		obb.UpdateOBB(GetComponentTransform());
	}
}

void UPrimitiveComponent::Render()
{
	URenderer* Renderer = UEngine::Get().GetRenderer();
	if (Renderer == nullptr || !GetVisibleFlag())
	{
		return;
	}
	if (GetOwner()->IsGizmoActor() == false)
	{
		if (bIsPicked)
		{
			/*bUseVertexColor = false;
			SetCustomColor(FVector4(1.0f, 0.647f, 0.0f, 1.0f));*/
		}
		else
		{
			bUseVertexColor = true;
		}
	}
	Renderer->RenderPrimitive(this);
}

// void UPrimitiveComponent::SetMaterial(UMaterial* NewMaterial)
// {
// 	// 이거의 타이밍? 
// 	// if (CurFrameData.Material != NewMaterial)
// 	// {
// 	// 	if (bIsDirty == false)
// 	// 	{
// 	// 		PrevFrameData = CurFrameData;
// 	// 		bIsDirty = true;
// 	// 	}
// 	// 	
// 	// 	CurFrameData.Material = NewMaterial;
// 	// }
// }
//
// void UPrimitiveComponent::SetTopology(D3D11_PRIMITIVE_TOPOLOGY NewTopologyType)
// {
// 	// if (CurFrameData.TopologyType != NewTopologyType)
// 	// {
// 	// 	if (bIsDirty == false)
// 	// 	{
// 	// 		PrevFrameData = CurFrameData;
// 	// 		bIsDirty = true;
// 	// 	}
// 	// 	CurFrameData.TopologyType = NewTopologyType;
// 	// }
//}

// void UPrimitiveComponent::UpdateConstantPicking(const URenderer& Renderer, const FVector4 UUIDColor)const
// {
// 	Renderer.UpdateConstantPicking(UUIDColor);
// }
//
// void UPrimitiveComponent::UpdateConstantDepth(const URenderer& Renderer, const int Depth)const
// {
// 	Renderer.UpdateConstantDepth(Depth);
// }

void UPrimitiveComponent::OnTransformation()
{
	if (GetRenderMode() == Batch)
		SetDirty(true);
}

//void UPrimitiveComponent::UpdateConstantUV(const URenderer& Renderer, const char c)const {
	//Renderer.UpdateConstantFontUV(c);
//}

// 배치 렌더링용 버텍스를 가져와서 
bool UPrimitiveComponent::TryGetVertexData(TArray<FVertexSimple>* VertexData)
{
	const TArray<FVertexSimple>* OriginVertexData = MeshResourceCache::Get().GetVertexData(GetMeshType());

	VertexData->Empty();
	if (OriginVertexData == nullptr)
	{
		return false;
	}

	for (const FVertexSimple& Vertex : *OriginVertexData)
	{
		FVertexSimple NewVertexSimple = Vertex;
		FVector Pos = FVector(Vertex.X, Vertex.Y, Vertex.Z) * GetComponentTransform().GetMatrix();
		NewVertexSimple.X = Pos.X;
		NewVertexSimple.Y = Pos.Y;
		NewVertexSimple.Z = Pos.Z;
		VertexData->Add(NewVertexSimple);
	}

	return true;
}
