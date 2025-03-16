#include "UPrimitiveComponent.h"
#include "Object/World/World.h"

void UPrimitiveComponent::Activate()
{
	Super::Activate();

	GetOwner()->GetWorld()->AddRenderComponent(this);
}

void UPrimitiveComponent::Deactivate()
{
	USceneComponent::Deactivate();

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
	aabb.UpdateAABB(GetComponentTransform(), GetType());
}

	CheckIsDirty();
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

void UPrimitiveComponent::UpdateConstantPicking(const URenderer& Renderer, const FVector4 UUIDColor)const
{
	Renderer.UpdateConstantPicking(UUIDColor);
}

void UPrimitiveComponent::UpdateConstantDepth(const URenderer& Renderer, const int Depth)const
{
	Renderer.UpdateConstantDepth(Depth);
}

void UPrimitiveComponent::CheckIsDirty()
{

	// 1. 변경사항이 있는가 -> batch나 Instancing이었던 경우 Update해줘야됨.

	// 2. 변경사항이 없다. ->

	// 2.1. 배치나 인스턴싱인 경우
	// 2.2. 개별 렌더링 -> 바로 렌더링

	
	if (bIsDirty)
	{
		
	}
	else
	{
		
	}
}