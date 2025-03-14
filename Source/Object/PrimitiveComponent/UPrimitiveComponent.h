#pragma once

#include "Core/Engine.h"
#include "Object/USceneComponent.h"
#include "Primitive/PrimitiveVertices.h"
#include "Core/Math/Plane.h"

struct FAABB {
	FVector Min;
	FVector Max;

	void GenerateAABB(EPrimitiveType type) {
		TArray<FVertexSimple> vertices = UEngine::Get().GetBufferCache()->GetVertexData(type);  
		FVector min = FVector(vertices[0].X, vertices[0].Y, vertices[0].Z);
		FVector max = FVector(vertices[0].X, vertices[0].Y, vertices[0].Z);
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

	void UpdateAABB(FTransform transform) {
		TArray<FVertexSimple> vertices = UEngine::Get().GetBufferCache()->GetVertexData(type);
		FVector min = FVector(vertices[0].X, vertices[0].Y, vertices[0].Z);
		FVector max = FVector(vertices[0].X, vertices[0].Y, vertices[0].Z);
		FMatrix model =
			FMatrix::GetScaleMatrix(transform.GetScale()) *
			FMatrix::GetRotateMatrix(transform.GetRotation()) *
			FMatrix::GetTranslateMatrix(transform.GetPosition());

		for (FVertexSimple vertex : vertices) {
			FVector4 pos = FVector(vertex.X, vertex.Y, vertex.Z, 1.0f);
			pos = model.TransformVector4(pos);
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
public:
	FAABB aabb;
public:
	UPrimitiveComponent() = default;
	virtual ~UPrimitiveComponent() = default;

public:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	void UpdateConstantPicking(const URenderer& Renderer, FVector4 UUIDColor) const;
	void UpdateConstantDepth(const URenderer& Renderer, int Depth) const;
	virtual void Render();
	
	virtual EPrimitiveMeshType GetType() { return EPrimitiveMeshType::EPT_None; }

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

public:
	virtual void RegisterComponentWithWorld(class UWorld* World);

public:

	void SetIsOrthoGraphic(bool IsOrtho) { bIsOrthoGraphic = IsOrtho; }
	bool GetIsOrthoGraphic() { return bIsOrthoGraphic;}
	bool GetIsBatch() {return bIsBatch; }

protected:
	bool bUseVertexColor = true;
	bool bIsOrthoGraphic = false;
	bool bIsBatch = false;
	FVector4 CustomColor = FVector4(1.0f, 1.0f, 1.0f, 1.0f);
};

class UCubeComp : public UPrimitiveComponent
{
	using Super = UPrimitiveComponent;
public:
	UCubeComp()
	{
		bCanBeRendered = true;
		aabb.GenerateAABB(EPrimitiveType::EPT_Cube);
	}
	virtual ~UCubeComp() = default;
	EPrimitiveMeshType GetType() override
	{
		return EPrimitiveMeshType::EPT_Cube;
	}
};

class USphereComp : public UPrimitiveComponent
{
	using Super = UPrimitiveComponent;
public:
	USphereComp()
	{
		bCanBeRendered = true;
		aabb.GenerateAABB(EPrimitiveType::EPT_Sphere);
	}
	virtual ~USphereComp() = default;
	EPrimitiveMeshType GetType() override
	{
		return EPrimitiveMeshType::EPT_Sphere;
	}
};

class UTriangleComp : public UPrimitiveComponent
{
	using Super = UPrimitiveComponent;
public:
	UTriangleComp()
	{
		bCanBeRendered = true;
		aabb.GenerateAABB(EPrimitiveType::EPT_Triangle);
	}
	virtual ~UTriangleComp() = default;
	EPrimitiveMeshType GetType() override
	{
		return EPrimitiveMeshType::EPT_Triangle;
	}
};

class ULineComp : public UPrimitiveComponent
{
	using Super = UPrimitiveComponent;

public:
	ULineComp()
	{
		bCanBeRendered = true;
		aabb.GenerateAABB(EPrimitiveType::EPT_Line);
	}
	virtual ~ULineComp() = default;
	EPrimitiveMeshType GetType() override
	{
		return EPrimitiveMeshType::EPT_Line;
	}
};

class UCylinderComp : public UPrimitiveComponent
{
	using Super = UPrimitiveComponent;

public:
	UCylinderComp()
	{
		bCanBeRendered = true;
		aabb.GenerateAABB(EPrimitiveType::EPT_Cylinder);
	}
	virtual ~UCylinderComp() = default;
	EPrimitiveMeshType GetType() override
	{
		return EPrimitiveMeshType::EPT_Cylinder;
	}
};

class UConeComp : public UPrimitiveComponent
{
	using Super = UPrimitiveComponent;
public:
	UConeComp()
	{
		bCanBeRendered = true;
		aabb.GenerateAABB(EPrimitiveType::EPT_Cone);
	}
	virtual ~UConeComp() = default;
	EPrimitiveMeshType GetType() override
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
		bCanBeRendered = false;
		aabb.GenerateAABB(EPrimitiveType::EPT_Box);
	}
	virtual ~UBoundingBoxComp() = default;
	EPrimitiveType GetType() override
	{
		return EPrimitiveType::EPT_Box;
	}
};