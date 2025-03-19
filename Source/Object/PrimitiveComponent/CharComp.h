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

    ////////////////////////////////////////////////
    ACamera* Camera = FEditorManager::Get().GetCamera();
    FVector CameraPos = Camera->GetActorTransform().GetPosition();
    FVector Offset = GetComponentTransform().GetPosition();
    FVector Pivot = GetAttachParent()->GetComponentTransform().GetPosition();
    FVector WorldUp = FVector(0, 0, 1);

    FVector LookVector = -FVector(Camera->GetActorTransform().GetPosition() - Offset).GetSafeNormal();
    FVector RightVector = LookVector.Cross(WorldUp).GetSafeNormal();
    FVector UpVector = LookVector.Cross(RightVector).GetSafeNormal();
    
    WorldUp = Camera->GetActorTransform().GetUp();
    
    FVector PivotLookVector = (Camera->GetActorTransform().GetPosition() - Pivot);
    FVector PivotRightVector = PivotLookVector.Cross(WorldUp).GetSafeNormal();
    FVector PivotUpVector = PivotLookVector.Cross(PivotRightVector).GetSafeNormal();


    FMatrix PivotRotateMatrix=FMatrix({
        { PivotLookVector.X, PivotLookVector.Y, PivotLookVector.Z,  0},	// forward
        { PivotRightVector.X, PivotRightVector.Y, PivotRightVector.Z, 0},		// right
        { PivotUpVector.X, PivotUpVector.Y, PivotUpVector.Z, 0 },// up
        {0, 0, 0, 1 },
    });
    FMatrix RotateMatrix=FMatrix({
        { LookVector.X, LookVector.Y, LookVector.Z,  0},	// forward
        { RightVector.X, RightVector.Y, RightVector.Z, 0},		// right
        { UpVector.X, UpVector.Y, UpVector.Z, 0 },// up
        {0, 0, 0, 1 },
    });

    // ✅ 1️⃣ Pivot과 Camera 사이 거리 계산
    float DistancePivotCamera = FVector::Distance(Pivot, CameraPos);

    // ✅ 2️⃣ 현재 Object와 Camera 사이 거리 계산
    float DistanceObjectCamera = FVector::Distance(Offset, CameraPos);
    
    FVector RelativeOffset = this->GetRelativeTransform().GetPosition();
    RelativeOffset=Pivot+RelativeOffset*PivotRotateMatrix;
    
    return RotateMatrix*FMatrix::GetTranslateMatrix(RelativeOffset);



}
