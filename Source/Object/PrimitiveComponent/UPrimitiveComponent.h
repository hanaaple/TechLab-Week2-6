#pragma once

#include "Core/Rendering/Material.h"
#include "Core/Rendering/URenderer.h"
#include "Object/USceneComponent.h"
#include "Primitive/PrimitiveVertices.h"
#include "Core/Math/Plane.h"

struct FAABB {
	FVector Min;
	FVector Max;

	void GenerateAABB(EPrimitiveMeshType type) {
		TArray<FVertexSimple> vertices = UEngine::Get().GetRenderer()->BufferCache->GetVertexData(type);
		FVector min = FVector(1000, 1000, 1000);
		FVector max = -min;
		for (const FVertexSimple& vertex : vertices) {
			min.X = FMath::Min(min.X, vertex.X);
			min.Y = FMath::Min(min.Y, vertex.Y);
			min.Z = FMath::Min(min.Z, vertex.Z);
			max.X = FMath::Max(max.X, vertex.X);
			max.Y = FMath::Max(max.Y, vertex.Y);
			max.Z = FMath::Max(max.Z, vertex.Z);
			UE_LOG("%f, %f, %f", vertex.X, vertex.Y, vertex.Z);
		}
		UE_LOG("min: %f, %f, %f", min.X, min.Y, min.Z);
		UE_LOG("max: %f, %f, %f", max.X, max.Y, max.Z);
		Min = min;
		Max = max;
	}

	void UpdateAABB(FTransform transform, EPrimitiveMeshType type) {
		TArray<FVertexSimple> vertices = UEngine::Get().GetRenderer()->BufferCache->GetVertexData(type);
		FVector min = FVector(1000, 1000, 1000);
		FVector max = -min;
		FMatrix model =
			FMatrix::GetScaleMatrix(transform.GetScale()) *
			FMatrix::GetRotateMatrix(transform.GetEulerRotation()) *
			FMatrix::GetTranslateMatrix(transform.GetPosition());

		for (const FVertexSimple& vertex : vertices) {
			FVector4 pos = FVector4(vertex.X, vertex.Y, vertex.Z, 1);
			
			
			pos = pos * model;
			min.X = FMath::Min(min.X, pos.X);
			min.Y = FMath::Min(min.Y, pos.Y);
			min.Z = FMath::Min(min.Z, pos.Z);
			max.X = FMath::Max(max.X, pos.X);
			max.Y = FMath::Max(max.Y, pos.Y);
			max.Z = FMath::Max(max.Z, pos.Z);
		}
		Min = min;
		Max = max;
	}
};


class UPrimitiveComponent : public USceneComponent
{
	using Super = USceneComponent;
    DECLARE_OBJECT(UPrimitiveComponent,Super)
public:
	UPrimitiveComponent() : Super(), Depth(0)
	{
	}
	FAABB aabb;
public:
	UPrimitiveComponent() = default;
	virtual ~UPrimitiveComponent() = default;

public:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void Render();
	virtual void Activate() override;
	virtual void Deactivate() override;

	
	void UpdateConstantPicking(const URenderer& Renderer, FVector4 UUIDColor) const;
	void UpdateConstantDepth(const URenderer& Renderer, int Depth) const;
	
	virtual EPrimitiveMeshType GetMeshType() { return EPrimitiveMeshType::EPT_None; }


public:
	void SetDepth(int InDepth) { Depth = InDepth; }
	int GetDepth() const { return Depth; }

	//FMaterial* Material;

	// Texture* texture;

	
	//UMaterial* GetMaterial() const { return CurFrameData.Material; }
	//void SetMaterial(UMaterial* NewMaterial);
	//void SetTopology(D3D11_PRIMITIVE_TOPOLOGY NewTopologyType);
	//D3D11_PRIMITIVE_TOPOLOGY GetTopology() const { return CurFrameData.TopologyType; }
	//ERenderMode GetERenderMode() const { return RenderMode; }

private:
	void CheckIsDirty();

protected:
	//FRenderData PrevFrameData;
	//FRenderData CurFrameData;
	
	bool bIsDirty;	// if Material or Topology .... Changes

public:
	void* Texture;

public:
	bool IsUseVertexColor() const { return bUseVertexColor; }

	void SetCustomColor(const FVector4& InColor)
	{
		CustomColor = InColor; 
		bUseVertexColor = false;
	}

	void SetUseVertexColor(bool bUse)
	{
		bUseVertexColor = bUse;
	}
	const FVector4& GetCustomColor() const { return CustomColor; }
	
protected:
	bool bUseVertexColor = true;
	FVector4 CustomColor = FVector4(1.0f, 1.0f, 1.0f, 1.0f);

private:
	uint32 Depth;
};

class UCubeComp : public UPrimitiveComponent
{
	using Super = UPrimitiveComponent;
	DECLARE_OBJECT(UCubeComp,Super)
public:
	UCubeComp()
	{
		SetVisibility(true);
		aabb.GenerateAABB(EPrimitiveMeshType::EPT_Cube);
	}
	virtual ~UCubeComp() = default;
	EPrimitiveMeshType GetMeshType() override
	{
		return EPrimitiveMeshType::EPT_Cube;
	}
};

class USphereComp : public UPrimitiveComponent
{
	using Super = UPrimitiveComponent;
	DECLARE_OBJECT(USphereComp,Super)
public:
	USphereComp()
	{
		SetVisibility(true);
		aabb.GenerateAABB(EPrimitiveMeshType::EPT_Sphere);
	}
	virtual ~USphereComp() = default;
	EPrimitiveMeshType GetMeshType() override
	{
		return EPrimitiveMeshType::EPT_Sphere;
	}
};

class UTriangleComp : public UPrimitiveComponent
{
	using Super = UPrimitiveComponent;
	DECLARE_OBJECT(UTriangleComp,Super)
public:
	UTriangleComp()
	{
		SetVisibility(true);
		aabb.GenerateAABB(EPrimitiveMeshType::EPT_Triangle);
	}
	virtual ~UTriangleComp() = default;
	EPrimitiveMeshType GetMeshType() override
	{
		return EPrimitiveMeshType::EPT_Triangle;
	}
};

class ULineComp : public UPrimitiveComponent
{
	using Super = UPrimitiveComponent;
	DECLARE_OBJECT(ULineComp,Super)

public:
	ULineComp()
	{
		SetVisibility(true);
		aabb.GenerateAABB(EPrimitiveMeshType::EPT_Line);
	}
	virtual ~ULineComp() = default;
	EPrimitiveMeshType GetMeshType() override
	{
		return EPrimitiveMeshType::EPT_Line;
	}
};

class UCylinderComp : public UPrimitiveComponent
{
	using Super = UPrimitiveComponent;
	DECLARE_OBJECT(UCylinderComp,Super)

public:
	UCylinderComp()
	{
		SetVisibility(true);
		aabb.GenerateAABB(EPrimitiveMeshType::EPT_Cylinder);
	}
	virtual ~UCylinderComp() = default;
	EPrimitiveMeshType GetMeshType() override
	{
		return EPrimitiveMeshType::EPT_Cylinder;
	}
};

class UConeComp : public UPrimitiveComponent
{
	using Super = UPrimitiveComponent;
	DECLARE_OBJECT(UConeComp,Super)
public:
	UConeComp()
	{
		SetVisibility(true);
		aabb.GenerateAABB(EPrimitiveMeshType::EPT_Cone);
	}
	virtual ~UConeComp() = default;
	EPrimitiveMeshType GetMeshType() override
	{
		return EPrimitiveMeshType::EPT_Cone;
	}
};


class UBoundingBoxComp : public UPrimitiveComponent
{
	using Super = UPrimitiveComponent;
public:
	UBoundingBoxComp()
	{
		SetVisibility(true);
		aabb.GenerateAABB(EPrimitiveMeshType::EPT_Box);
	}
	virtual ~UBoundingBoxComp() = default;
	EPrimitiveMeshType GetType() override
	{
		return EPrimitiveMeshType::EPT_Box;
	}
};