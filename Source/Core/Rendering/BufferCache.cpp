#include "BufferCache.h"

#include <ranges>

#include "Core/Engine.h"
#include "Primitive/PrimitiveVertices.h"

FBufferCache::FBufferCache()
{
}

FBufferCache::~FBufferCache()
{
}

BufferInfo& FBufferCache::GetVertexBufferInfo(EPrimitiveComponentType Type)
{
	if (!VertexBufferCache.contains(Type))
	{
		//여기서 버텍스 버퍼 생성한다
		auto bufferInfo =	CreateVertexBufferInfo(Type);
		VertexBufferCache.insert({ Type, bufferInfo });
	}

	return VertexBufferCache[Type];
}

BufferInfo& FBufferCache::GetIndexBufferInfo(EPrimitiveComponentType Type)
{
	if (!IndexBufferCache.contains(Type))
	{
		// TODO TEMP default인 경우
		//여기서 인덱스 버퍼 생성한다
		auto bufferInfo = CreateIndexBufferInfo(Type);
		IndexBufferCache.insert({ Type, bufferInfo });
	}

	return IndexBufferCache[Type];
}

void FBufferCache::Release()
{
	for (auto& BufferInfo : IndexBufferCache | std::views::values)
	{
		BufferInfo.GetBuffer()->Release();
	}

	for (auto& BufferInfo : VertexBufferCache | std::views::values)
	{
		BufferInfo.GetBuffer()->Release();
	}

	IndexBufferCache.clear();
	VertexBufferCache.clear();
}

BufferInfo FBufferCache::CreateVertexBufferInfo(EPrimitiveComponentType Type)
{
	ID3D11Buffer* Buffer = nullptr;
	int Size = 0;
	D3D_PRIMITIVE_TOPOLOGY Topology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	switch (Type)
	{
	case EPrimitiveComponentType::EPT_Line:
		Size = std::size(LineVertices);
		Buffer = UEngine::Get().GetRenderer()->CreateVertexBuffer(LineVertices, sizeof(FVertexSimple) * Size, D3D11_BIND_VERTEX_BUFFER, D3D11_USAGE_IMMUTABLE);
		Topology = D3D_PRIMITIVE_TOPOLOGY_LINELIST;
		break;
	case EPrimitiveComponentType::EPT_Triangle:
		Size = std::size(TriangleVertices);
		Buffer = UEngine::Get().GetRenderer()->CreateVertexBuffer(TriangleVertices, sizeof(FVertexSimple) * Size, D3D11_BIND_VERTEX_BUFFER, D3D11_USAGE_IMMUTABLE);
		break;
	case EPrimitiveComponentType::EPT_Cube:
		Size = std::size(CubeVertices);
		Buffer = UEngine::Get().GetRenderer()->CreateVertexBuffer(CubeVertices, sizeof(FVertexSimple) * Size, D3D11_BIND_VERTEX_BUFFER, D3D11_USAGE_IMMUTABLE);
		break;
	case EPrimitiveComponentType::EPT_Sphere:
		Size = std::size(SphereVertices);
		Buffer = UEngine::Get().GetRenderer()->CreateVertexBuffer(SphereVertices, sizeof(FVertexSimple) * Size, D3D11_BIND_VERTEX_BUFFER, D3D11_USAGE_IMMUTABLE);
		break;
	case EPrimitiveComponentType::EPT_Cylinder:
	{
		TArray<FVertexSimple> Vertices = CreateCylinderVertices();
		Size = Vertices.Num();
		Buffer = UEngine::Get().GetRenderer()->CreateVertexBuffer(Vertices.GetData(), sizeof(FVertexSimple) * Size, D3D11_BIND_VERTEX_BUFFER, D3D11_USAGE_IMMUTABLE);
		break;
	}
	case EPrimitiveComponentType::EPT_Cone:
	{
		TArray<FVertexSimple> Vertices = CreateConeVertices();
		Size = Vertices.Num();
		Buffer = UEngine::Get().GetRenderer()->CreateVertexBuffer(Vertices.GetData(), sizeof(FVertexSimple) * Size, D3D11_BIND_VERTEX_BUFFER, D3D11_USAGE_IMMUTABLE);
		break;
	}
	}

	return BufferInfo(Buffer, Size, Topology);
}

BufferInfo FBufferCache::CreateIndexBufferInfo(EPrimitiveComponentType Type)
{
	ID3D11Buffer* Buffer = nullptr;
	int Size = 0;
	D3D_PRIMITIVE_TOPOLOGY Topology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	switch (Type)
	{
	// case EPrimitiveType::EPT_Line:
	// 	Size = std::size(LineVertices);
	// 	Buffer = UEngine::Get().GetRenderer()->CreateIndexBuffer(LineVertices, sizeof(FVertexSimple) * Size, TODO, TODO);
	// 	Topology = D3D_PRIMITIVE_TOPOLOGY_LINELIST;
	// 	break;
	// case EPrimitiveType::EPT_Triangle:
	// 	Size = std::size(TriangleVertices);
	// 	Buffer = UEngine::Get().GetRenderer()->CreateIndexBuffer(TriangleVertices, sizeof(FVertexSimple) * Size, TODO, TODO);
	// 	break;
	case EPrimitiveComponentType::EPT_Cube:
		Size = CubeIndecies.Num();
		Buffer = UEngine::Get().GetRenderer()->CreateIndexBuffer(CubeIndecies, sizeof(uint32) * Size, D3D11_BIND_INDEX_BUFFER, D3D11_USAGE_IMMUTABLE);
		break;
	// case EPrimitiveType::EPT_Sphere:
	// 	//Size = std::size(SphereVertices);
	// 	Buffer = UEngine::Get().GetRenderer()->CreateIndexBuffer(SphereVertices, sizeof(FVertexSimple) * Size, TODO, TODO);
	// 	break;
	// case EPrimitiveType::EPT_Cylinder:
	// 	{
	// 		//TArray<FVertexSimple> Vertices = CreateCylinderVertices();
	// 		Size = Vertices.Num();
	// 		Buffer = UEngine::Get().GetRenderer()->CreateIndexBuffer(Vertices.GetData(), sizeof(FVertexSimple) * Size, TODO, TODO);
	// 		break;
	// 	}
	// case EPrimitiveType::EPT_Cone:
	// 	{
	// 		//TArray<FVertexSimple> Vertices = CreateConeVertices();
	// 		Size = Vertices.Num();
	// 		Buffer = UEngine::Get().GetRenderer()->CreateIndexBuffer(Vertices.GetData(), sizeof(FVertexSimple) * Size, TODO, TODO);
	// 		break;
	// 	}
		// TODO TEMP
	default:
		return BufferInfo();
	}

	return BufferInfo(Buffer, Size, Topology);	
}


TArray<FVertexSimple> FBufferCache::CreateConeVertices()
{
	TArray<FVertexSimple> vertices;

	int segments = 36;
	float radius = 1.f;
	float height = 1.f;


	// 원뿔의 바닥
	for (int i = 0; i < segments; ++i)
	{
		float angle = 2.0f * PI * i / segments;
		float nextAngle = 2.0f * PI * (i + 1) / segments;

		float x1 = radius * cos(angle);
		float y1 = radius * sin(angle);
		float x2 = radius * cos(nextAngle);
		float y2 = radius * sin(nextAngle);

		 // 바닥 삼각형 (반시계 방향으로 추가)
        vertices.Add({ 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f });
        vertices.Add({ x2, y2, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f });
        vertices.Add({ x1, y1, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f });

        // 옆면 삼각형 (시계 방향으로 추가)
        vertices.Add({ x1, y1, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f });
        vertices.Add({ x2, y2, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f });
        vertices.Add({ 0.0f, 0.0f, height, 0.0f, 1.0f, 0.0f, 1.0f });
	}

	return vertices;
}

TArray<FVertexSimple> FBufferCache::CreateCylinderVertices()
{
	TArray<FVertexSimple> vertices;
	
	int segments = 36;
	float radius = .03f;
	float height = .5f;


	// 원기둥의 바닥과 윗면
	for (int i = 0; i < segments; ++i)
	{
		float angle = 2.0f * PI * i / segments;
		float nextAngle = 2.0f * PI * (i + 1) / segments;

		float x1 = radius * cos(angle);
		float y1 = radius * sin(angle);
		float x2 = radius * cos(nextAngle);
		float y2 = radius * sin(nextAngle);

		// 바닥 삼각형
		vertices.Add({ 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f });
		vertices.Add({ x2, y2, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f });
		vertices.Add({ x1, y1, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f });

		// 윗면 삼각형
		vertices.Add({ 0.0f, 0.0f, height, 0.0f, 1.0f, 0.0f, 1.0f });
		vertices.Add({ x1, y1, height, 0.0f, 1.0f, 0.0f, 1.0f });
		vertices.Add({ x2, y2, height, 0.0f, 1.0f, 0.0f, 1.0f });

		// 옆면 삼각형 두 개
		vertices.Add({ x1, y1, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f });
		vertices.Add({ x2, y2, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f });
		vertices.Add({ x1, y1, height, 0.0f, 0.0f, 1.0f, 1.0f });

		vertices.Add({ x2, y2, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f });
		vertices.Add({ x2, y2, height, 0.0f, 0.0f, 1.0f, 1.0f });
		vertices.Add({ x1, y1, height, 0.0f, 0.0f, 1.0f, 1.0f });
	}

	return vertices;
}
