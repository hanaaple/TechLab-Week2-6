#pragma once

#include "Core/Rendering/URenderer.h"
#include "DataTypes/MeshDataTypes.h"
#include "Object/USceneComponent.h"
#include <Core/Engine.h>

struct FAABB {
	FVector Min = FVector(FLT_MAX, FLT_MAX, FLT_MAX);
	FVector Max = -Min;

	void UpdateAABB(TArray<FVertexSimple>& vertices) {

		FVector min = FVector(FLT_MAX, FLT_MAX, FLT_MAX);
		FVector max = -min;
		for (const FVertexSimple& vertex : vertices) {
			min.X = FMath::Min(min.X, vertex.X);
			min.Y = FMath::Min(min.Y, vertex.Y);
			min.Z = FMath::Min(min.Z, vertex.Z);
			max.X = FMath::Max(max.X, vertex.X);
			max.Y = FMath::Max(max.Y, vertex.Y);
			max.Z = FMath::Max(max.Z, vertex.Z);
		}
		Min = min;
		Max = max;
	}
};


enum ERenderMode{
	Batch,
	Individual,
	// Instancing
};

struct FRenderData
{
	ID3D11ShaderResourceView* Texture = nullptr;
	D3D_PRIMITIVE_TOPOLOGY Topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	bool bUseIndexBuffer = false;
	EPrimitiveMeshType MeshType = EPrimitiveMeshType::EPT_None;
	ERenderMode RenderMode = ERenderMode::Individual;
	FName ShaderName = nullptr;
};

class UPrimitiveComponent : public USceneComponent
{
	using Super = USceneComponent;
	DECLARE_OBJECT(UPrimitiveComponent,Super)

public:
	FAABB aabb;
public:
	UPrimitiveComponent():Super(), Depth(0){}
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

	// void UpdateConstantPicking(const URenderer& Renderer, FVector4 UUIDColor) const;
	// void UpdateConstantDepth(const URenderer& Renderer, int Depth) const;

	virtual bool TryGetVertexData(TArray<FVertexSimple>* VertexData);

public:
	void SetDepth(int InDepth) { Depth = InDepth; }
	int GetDepth() const { return Depth; }

	void SetDirty(bool NewIsDirty)
	{
		bIsDirty = NewIsDirty;
		if (NewIsDirty == false){
			PrevFrameData = CurrentRenderData;
		}
	}
	int GetIsDirty() const { return bIsDirty; }

	const FRenderData& GetPrevFrameData() const { return PrevFrameData; }
	const FRenderData& GetCurFrameData() const { return CurrentRenderData; }

	void SetTexture(ID3D11ShaderResourceView* NewTexture)
	{
		if (CurrentRenderData.Texture != NewTexture)
			SetDirty(true);
		CurrentRenderData.Texture = NewTexture;		
	}
	void SetMesh(EPrimitiveMeshType NewMeshType)
	{
		if (CurrentRenderData.MeshType != NewMeshType)
			SetDirty(true);
		CurrentRenderData.MeshType = NewMeshType;
	}
	void SetUseIndexBuffer(bUseIndexBufferFlag bUseIndexBuffer)
	{
		if (CurrentRenderData.bUseIndexBuffer != bUseIndexBuffer)
			SetDirty(true);
		CurrentRenderData.bUseIndexBuffer = bUseIndexBuffer;
	}
	void SetTopology(D3D11_PRIMITIVE_TOPOLOGY NewTopologyType)
	{
		if (CurrentRenderData.Topology != NewTopologyType)
			SetDirty(true);
		CurrentRenderData.Topology = NewTopologyType;
	}
	void SetRenderMode(ERenderMode NewMode)
	{
		if (CurrentRenderData.RenderMode != NewMode)
			SetDirty(true);
		
		CurrentRenderData.RenderMode = NewMode;
	}
	void SetShaderName(FName NewShaderName)
	{
		if (CurrentRenderData.ShaderName != NewShaderName)
			SetDirty(true);
		
		CurrentRenderData.ShaderName = NewShaderName;
	}

	FName GetShaderName() const { return CurrentRenderData.ShaderName; }
	
	EPrimitiveMeshType GetMeshType() const { return CurrentRenderData.MeshType; }

	ERenderMode GetRenderMode() const { return CurrentRenderData.RenderMode; }

	
	
	D3D11_PRIMITIVE_TOPOLOGY GetTopology() const { return CurrentRenderData.Topology; }
	ID3D11ShaderResourceView* GetTexture() const { return CurrentRenderData.Texture; }
	
	
public:
	bool IsUseVertexColor() const { return bUseVertexColor; }
	bool IsUseTexture() const { return bUseUV; }
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
	bool bUseUV = false;
	FVector4 CustomColor = FVector4(1.0f, 1.0f, 1.0f, 1.0f);

private:
	FRenderData PrevFrameData = FRenderData();
	FRenderData CurrentRenderData = FRenderData();
	uint32 Depth;
	bool bIsDirty;
};

class UCubeComp : public UPrimitiveComponent
{
	using Super = UPrimitiveComponent;
	DECLARE_OBJECT(UCubeComp,Super)
public:
	UCubeComp() : Super()
	{
		SetMesh(EPrimitiveMeshType::EPT_Cube);
		UTextureLoader::Get().LoadTexture("Resources/tempTexture.png");
		SetTexture(UTextureLoader::Get().m_texture);
	}
	virtual ~UCubeComp() = default;
};

class USphereComp : public UPrimitiveComponent
{
	using Super = UPrimitiveComponent;
	DECLARE_OBJECT(USphereComp,Super)
public:
	USphereComp() : Super()
	{
		SetMesh(EPrimitiveMeshType::EPT_Sphere);
	}
	virtual ~USphereComp() = default;
};

class UTriangleComp : public UPrimitiveComponent
{
	using Super = UPrimitiveComponent;
	DECLARE_OBJECT(UTriangleComp,Super)
public:
	UTriangleComp() : Super()
	{
		SetMesh(EPrimitiveMeshType::EPT_Triangle);
	}
	virtual ~UTriangleComp() = default;
};

class ULineComp : public UPrimitiveComponent
{
	using Super = UPrimitiveComponent;
	DECLARE_OBJECT(ULineComp,Super)

public:
	ULineComp() : Super()
	{
		SetMesh(EPrimitiveMeshType::EPT_Line);
		SetTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
	}
	virtual ~ULineComp() = default;
};

class UCylinderComp : public UPrimitiveComponent
{
	using Super = UPrimitiveComponent;
	DECLARE_OBJECT(UCylinderComp,Super)

public:
	UCylinderComp() : Super()
	{
		SetMesh(EPrimitiveMeshType::EPT_Cylinder);
	}
	virtual ~UCylinderComp() = default;
};

class UConeComp : public UPrimitiveComponent
{
	using Super = UPrimitiveComponent;
	DECLARE_OBJECT(UConeComp,Super)
public:
	UConeComp() : Super()
	{
		SetMesh(EPrimitiveMeshType::EPT_Cone);
	}
	virtual ~UConeComp() = default;
};

class UTorusComp : public UPrimitiveComponent
{
	using Super = UPrimitiveComponent;
	DECLARE_OBJECT(UTorusComp,Super)
public:
	UTorusComp() : Super()
	{
		SetMesh(EPrimitiveMeshType::EPT_Torus);
	}
	virtual ~UTorusComp() = default;
};


class UBoundingBoxComp : public UPrimitiveComponent
{
	using Super = UPrimitiveComponent;
	DECLARE_OBJECT(UBoundingBoxComp, Super)
public:
	UBoundingBoxComp()
	{
		SetMesh(EPrimitiveMeshType::EPT_BoundingBox);
		SetTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
	}
	virtual ~UBoundingBoxComp() = default;
};