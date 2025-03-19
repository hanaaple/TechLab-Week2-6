#pragma once
#include "UPrimitiveComponent.h"
#include "Static/FEditorManager.h"

class UCharComp : public UPrimitiveComponent {
	using Super = UPrimitiveComponent;
	DECLARE_OBJECT(UCharComp, Super)

public:
	UCharComp() : Super() {
		//bUseUV = true;
		
		SetTexture(ETextureType::FontTexture);
		SetMesh(EPrimitiveMeshType::EPT_Quad);
		SetTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		SetShaderType(EShaderType::TextShader);
	}
	bool TryGetVertexData(TArray<FVertexSimple>* VertexData) override;
public:
	FMatrix GetBillboardMatrix();
	
	char c;
};

inline bool UCharComp::TryGetVertexData(TArray<FVertexSimple>* VertexData)
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
		FVector Pos = FVector(Vertex.X, Vertex.Y, Vertex.Z) * GetBillboardMatrix();
		NewVertexSimple.X = Pos.X;
		NewVertexSimple.Y = Pos.Y;
		NewVertexSimple.Z = Pos.Z;
		VertexData->Add(NewVertexSimple);
	}

	return true;
}

inline FMatrix UCharComp::GetBillboardMatrix()
{
	//FVector Pivot = GetAttachParent()->GetComponentTransform().GetPosition();
	FVector Offset = GetComponentTransform().GetPosition();
	ACamera* Camera = FEditorManager::Get().GetCamera();
	//Pivot += Offset;
	FVector LookVector = -FVector(Camera->GetActorTransform().GetPosition() - Offset).GetSafeNormal();
	//Pivot -= Offset;
	FVector RightVector = LookVector.Cross(Camera->GetUp()).GetSafeNormal();
	FVector UpVector = LookVector.Cross(RightVector);

	return (FMatrix({
		{ LookVector.X, LookVector.Y, LookVector.Z,  0},	// forward
		{ RightVector.X, RightVector.Y, RightVector.Z, 0},		// right
		{ UpVector.X, UpVector.Y, UpVector.Z, 0 },// up
		{Offset.X, Offset.Y, Offset.Z, 1 },
	}));
}