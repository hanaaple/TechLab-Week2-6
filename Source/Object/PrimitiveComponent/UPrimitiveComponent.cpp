#include "UPrimitiveComponent.h"
#include "Object/World/World.h"

void UPrimitiveComponent::Activate()
{
	Super::Activate();

	GetOwner()->GetWorld()->AddRenderComponent(this);
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
	aabb.UpdateAABB(GetComponentTransform(), GetMeshType());
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


// 배치 렌더링용 버텍스를 가져와서 
TArray<FVertexSimple> UPrimitiveComponent::GetVertexData()
{
	TArray<FVertexSimple> VertexData = UEngine::Get().GetRenderer()->GetBufferCache()->GetStaticVertexData(GetMeshType());

	for (FVertexSimple& Vertex : VertexData)
	{
		FVector Pos = FVector(Vertex.X, Vertex.Y, Vertex.Z) * GetComponentTransform().GetMatrix();
		Vertex.X = Pos.X;
		Vertex.Y = Pos.Y;
		Vertex.Z = Pos.Z;
	}
	
	return VertexData;
}