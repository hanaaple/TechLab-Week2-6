#pragma once

#include "Core/Rendering/Material.h"
#include "Core/Rendering/URenderer.h"
#include "Object/USceneComponent.h"


class UPrimitiveComponent : public USceneComponent
{
	using Super = USceneComponent;
    DECLARE_OBJECT(UPrimitiveComponent,Super)
public:
	UPrimitiveComponent() : Super(), Depth(0)
	{
	}
	virtual ~UPrimitiveComponent() = default;

public:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void Render();
	virtual void Activate() override;
	virtual void Deactivate() override;

	
	void UpdateConstantPicking(const URenderer& Renderer, FVector4 UUIDColor) const;
	void UpdateConstantDepth(const URenderer& Renderer, int Depth) const;
	void UpdateConstantUV(const URenderer& Renderer, const char c)const;

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
	ID3D11ShaderResourceView* Texture;

public:
	bool IsUseVertexColor() const { return bUseVertexColor; }
	bool IsUseUV() const { return bUseUV; }
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
	bool bUseUV = true;
	FVector4 CustomColor = FVector4(1.0f, 1.0f, 1.0f, 1.0f);

private:
	uint32 Depth;
};

class UCubeComp : public UPrimitiveComponent
{
	using Super = UPrimitiveComponent;
	DECLARE_OBJECT(UCubeComp,Super)
public:
	UCubeComp() = default;
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
	USphereComp() = default;
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
	UTriangleComp() = default;
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
	ULineComp() = default;
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
	UCylinderComp() = default;
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
	UConeComp() : Super() {

		UTextureLoader::Get().LoadTexture("Resources/tempTexture.png");
		Texture = UTextureLoader::Get().m_texture;
	}
	virtual ~UConeComp() = default;
	EPrimitiveMeshType GetMeshType() override
	{
		return EPrimitiveMeshType::EPT_Cone;
	}
};
