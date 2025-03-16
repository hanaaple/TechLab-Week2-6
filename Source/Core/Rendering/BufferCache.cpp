#include "BufferCache.h"

#include <ranges>

#include "URenderer.h"
#include "Core/Engine.h"
#include "Primitive/PrimitiveVertices.h"

FBufferCache::FBufferCache()
{
}

FBufferCache::~FBufferCache()
{
}

BufferInfo& FBufferCache::GetVertexBufferInfo(EPrimitiveMeshType Type)
{
	if (!VertexBufferCache.contains(Type))
	{
		//여기서 버텍스 버퍼 생성한다
		auto bufferInfo = CreateVertexBufferInfo(Type);
		VertexBufferCache.insert({ Type, bufferInfo });
	}

	return VertexBufferCache[Type];
}

BufferInfo& FBufferCache::GetIndexBufferInfo(EPrimitiveMeshType Type)
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

BufferInfo& FBufferCache::GetVertexBufferInfo(ID3D11ShaderResourceView* Texture, D3D11_PRIMITIVE_TOPOLOGY Topology)
{
	if (BatchVertexBufferCache.Contains(Texture))
	{
		if (BatchVertexBufferCache[Texture].Contains(Topology))
		{
			return BatchVertexBufferCache[Texture][Topology];
		}
	}
	BufferInfo bufferInfo;
	return bufferInfo;
}

BufferInfo& FBufferCache::GetIndexBufferInfo(ID3D11ShaderResourceView* Texture, D3D11_PRIMITIVE_TOPOLOGY Topology)
{
	if (BatchIndexBufferCache.Contains(Texture))
	{
		if (BatchIndexBufferCache[Texture].Contains(Topology))
		{
			return BatchIndexBufferCache[Texture][Topology];
		}
	}
	BufferInfo bufferInfo;
	return bufferInfo;
}

void FBufferCache::UpdateVertexBuffer(ID3D11ShaderResourceView* Texture, D3D11_PRIMITIVE_TOPOLOGY Topology, ID3D11Buffer* Buffer)
{
	// TODO Batch - 고쳐야됨
	if (!BatchVertexBufferCache.Contains(Texture))
	{
		BatchVertexBufferCache.Add(Texture, TMap<D3D11_PRIMITIVE_TOPOLOGY, BufferInfo>());
	}
	if (!BatchVertexBufferCache[Texture].Contains(Topology))
	{
		//BatchVertexBufferCache[Texture].Add(Topology, );
	}
	BatchVertexBufferCache[Texture][Topology].GetBuffer()->Release();	
	//BatchVertexBufferCache[Texture][Topology] = BufferInfo(Buffer, BufferSize);
}

void FBufferCache::UpdateIndexBuffer(ID3D11ShaderResourceView* Texture, D3D11_PRIMITIVE_TOPOLOGY Topology, ID3D11Buffer* Buffer)
{
	// TODO Batch - 고쳐야됨
	if (BatchIndexBufferCache.Contains(Texture))
	{
		if (BatchIndexBufferCache[Texture].Contains(Topology))
		{
			BatchVertexBufferCache[Texture][Topology].GetBuffer()->Release();
			//BatchIndexBufferCache[Texture][Topology] = BufferInfo(Buffer, BufferSize);
		}
	}
}

TArray<uint32> FBufferCache::GetStaticIndexData(EPrimitiveMeshType MeshType)
{
	TArray<uint32> Indecies;
	switch (MeshType)
	{
	case EPrimitiveMeshType::EPT_Cube:
		Indecies = CubeIndecies;
		break;
	default:
		break;
	}

	return Indecies;
}

TArray<FVertexSimple> FBufferCache::GetStaticVertexData(EPrimitiveMeshType MeshType)
{
	TArray<FVertexSimple> VertexData;
	FVertexSimple* Vertices = nullptr;
	int Size = 0;
	switch (MeshType)
	{
	case EPrimitiveMeshType::EPT_Line:
		Vertices = LineVertices;
		Size = std::size(LineVertices);
		break;
	case EPrimitiveMeshType::EPT_Triangle:
		Vertices = TriangleVertices;
		Size = std::size(TriangleVertices);
		break;
	case EPrimitiveMeshType::EPT_Cube:
		Vertices = CubeVertices;
		Size = std::size(CubeVertices);
		break;
	case EPrimitiveMeshType::EPT_Sphere:
		Vertices = SphereVertices;
		Size = std::size(SphereVertices);
		break;
	case EPrimitiveMeshType::EPT_Cylinder:
		{
			VertexData = CreateCylinderVertices();
			break;
		}
	case EPrimitiveMeshType::EPT_Cone:
		{
			VertexData = CreateConeVertices();
			break;
		}
	default:
		// ERROR
			break;
	}
	
	VertexData.Append(Vertices, Size);
	
	return VertexData;
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

BufferInfo FBufferCache::CreateVertexBufferInfo(EPrimitiveMeshType Type)
{
	TArray<FVertexSimple> VertexData;
	ID3D11Buffer* Buffer = nullptr;
	int Size = 0;

	D3D11_BIND_FLAG BindFlag = D3D11_BIND_VERTEX_BUFFER;
	D3D11_USAGE D3d11Usage = D3D11_USAGE_IMMUTABLE;
	FVertexSimple* Vertices = nullptr;
	switch (Type)
	{
	case EPrimitiveMeshType::EPT_Line:
		Vertices = LineVertices;
		Size = std::size(LineVertices);
		break;
	case EPrimitiveMeshType::EPT_Triangle:
		Vertices = TriangleVertices;
		Size = std::size(TriangleVertices);
		break;
	case EPrimitiveMeshType::EPT_Cube:
		Vertices = CubeVertices;
		Size = std::size(CubeVertices);
		break;
	case EPrimitiveMeshType::EPT_Sphere:
		Vertices = SphereVertices;
		Size = std::size(SphereVertices);
		break;
	case EPrimitiveMeshType::EPT_Cylinder:
		{
			VertexData = CreateCylinderVertices();
			break;
		}
	case EPrimitiveMeshType::EPT_Cone:
		{
			VertexData = CreateConeVertices();
			break;
		}
	default:
		break;
		// ERROR
	}
	
	VertexData.Append(Vertices, Size);
	uint32 VertexSize = VertexData.Num();
	if (VertexData.Num() > 0)
		Buffer = UEngine::Get().GetRenderer()->CreateMeshBuffer(VertexData.GetData(), sizeof(FVertexSimple) * VertexSize, BindFlag, D3d11Usage);
	
	return BufferInfo(Buffer, VertexSize);	
}

BufferInfo FBufferCache::CreateIndexBufferInfo(EPrimitiveMeshType Type)
{
	ID3D11Buffer* Buffer = nullptr;
	uint32 Size = 0;
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
	case EPrimitiveMeshType::EPT_Cube:
		Size = CubeIndecies.Num();
		Buffer = UEngine::Get().GetRenderer()->CreateMeshBuffer(CubeIndecies.GetData(), sizeof(uint32) * Size, D3D11_BIND_INDEX_BUFFER, D3D11_USAGE_IMMUTABLE);
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

	return BufferInfo(Buffer, Size);	
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
