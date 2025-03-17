#include "BufferCache.h"

#include <ranges>

#include "URenderer.h"
#include "Core/Engine.h"

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
		if (bufferInfo.GetSize() <= 0)
		{
			BufferInfo bufferInfo;
			return bufferInfo;
		}
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

BufferInfo FBufferCache::CreateVertexBufferInfo(EPrimitiveMeshType MeshType)
{

	TArray<FVertexSimple>* VertexData = MeshResourceCache::Get().GetVertexData(MeshType);
	uint32 Size = 0;
	ID3D11Buffer* Buffer = nullptr;
	if (VertexData != nullptr)
	{
		Size = VertexData->Num();
		Buffer = UEngine::Get().GetRenderer()->CreateMeshBuffer(VertexData->GetData(), sizeof(FVertexSimple) * Size, D3D11_BIND_VERTEX_BUFFER, D3D11_USAGE_IMMUTABLE);
	}
	
	return BufferInfo(Buffer, Size);
}

BufferInfo FBufferCache::CreateIndexBufferInfo(EPrimitiveMeshType MeshType)
{
	TArray<uint32>* IndexData = MeshResourceCache::Get().GetIndexData(MeshType);

	ID3D11Buffer* Buffer = nullptr;
	uint32 Size = 0;
	if (IndexData != nullptr)
	{
		Size = IndexData->Num();
		Buffer = UEngine::Get().GetRenderer()->CreateMeshBuffer(IndexData->GetData(), sizeof(uint32) * Size, D3D11_BIND_INDEX_BUFFER, D3D11_USAGE_IMMUTABLE);
	}
	return BufferInfo(Buffer, Size);
}