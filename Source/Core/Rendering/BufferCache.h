#pragma once

#define _TCHAR_DEFINED
#include <d3d11.h>

#include <wrl/client.h>
#include <unordered_map>

#include "BatchRenderContext.h"
#include "Primitive/MeshResourceCache.h"

struct BufferInfo
{
public:
	BufferInfo() = default;
	BufferInfo(ID3D11Buffer* InBuffer, uint32 BufferSize)
	{
		Buffer = InBuffer;
		Size = BufferSize;
	}

	ID3D11Buffer* GetBuffer() const { return Buffer.Get(); }
	uint32 GetSize() const { return Size; }

private:
	Microsoft::WRL::ComPtr<ID3D11Buffer> Buffer = nullptr;
	uint32 Size = 0;
};


// Vertex Buffer, Index Buffer Cache
class FBufferCache
{
private:
	// 개별 렌더링 버퍼 (기본 메시 버퍼)
	std::unordered_map <EPrimitiveMeshType, BufferInfo> VertexBufferCache;
	std::unordered_map <EPrimitiveMeshType, BufferInfo> IndexBufferCache;

	// Batch 렌더링 버퍼 캐시
	TMap<ETextureType, TMap<D3D11_PRIMITIVE_TOPOLOGY, BufferInfo>> BatchIndexBufferCache;
	TMap<ETextureType, TMap<D3D11_PRIMITIVE_TOPOLOGY, BufferInfo>> BatchVertexBufferCache;
public:
	FBufferCache();
	~FBufferCache();

	BufferInfo& GetVertexBufferInfo(EPrimitiveMeshType Type);
	BufferInfo& GetIndexBufferInfo(EPrimitiveMeshType Type);

	BufferInfo& GetVertexBufferInfo(ETextureType TextureType, D3D11_PRIMITIVE_TOPOLOGY Topology);
	BufferInfo& GetIndexBufferInfo(ETextureType TextureType, D3D11_PRIMITIVE_TOPOLOGY Topology);

	void UpdateVertexBuffer(ETextureType TextureType, D3D11_PRIMITIVE_TOPOLOGY Topology, ID3D11Buffer* Buffer, uint32 BufferSize);
	void UpdateIndexBuffer(ETextureType TextureType, D3D11_PRIMITIVE_TOPOLOGY Topology, ID3D11Buffer* Buffer, uint32 BufferSize);
	
	void Release();

private :
	BufferInfo CreateVertexBufferInfo(EPrimitiveMeshType MeshType);
	BufferInfo CreateIndexBufferInfo(EPrimitiveMeshType MeshType);
};